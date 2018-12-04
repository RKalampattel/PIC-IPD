//
//  species.hpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Last edited by Rahul Kalampattel, March 2018
// 
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef species_hpp
#define species_hpp

#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <stdio.h>
#include <vector>
#include <string>

#include "../COMM/map1D.hpp"
#include "../COMM/assert_mk.hpp"
#include "../COMM/readdatafromstring.hpp"
#include "../COMM/error_codes.hpp"
#include "../COMM/error_exception.hpp"
#include "../COMM/StringOps.hpp"
#include "../COMM/VectorCompare.hpp"

#include "chemConstants.hpp"
#include "electronicState.hpp"
#include "kev.hpp"
#include "LeRc.hpp"
#include "Blottner.hpp"
#include "Sutherland.hpp"
#include "viscosity_kinetic_theory.hpp"




// 1. Basic DATA
class speciesBasic {
public:
    double M;       // MW (AMU)
    double m;       // mass in kg
    double h0;      // Enthalpy of formation [J/kg]
    double D;       // Dissociation energy [J/kg]
    double I;       // Ionisation energy [j/kg]
    double q;       // Species charge
    double R;       // Gas constant
    int    type;    // Type of species [0: atom/neutral, +1: ion, -1: electron]
    double Cv[2];   // Cv[0]: Cv_tra, Cv[1] = Cv_rot;
    
    LeRc lerc;      // Normalized enthalpy and entrophy data
    
protected:
    bool m_completed;
    
public:
    speciesBasic();
    ~speciesBasic();
    
    void read(const std::string& line);
};


// 2. Nonequilibrium DATA
class speciesNoneq
{
public:
    double Z_inf;
    double T_star;
    double T_ref;
    double omega;
    double d_ref;
    double q_rot_e;
    double theta_v;
    
    electronicState ele;
    relaxation_rate_e_v k_ev;
    
    
public:
    speciesNoneq();
    ~speciesNoneq();
    
    void read(const std::string& line);
    void read(const std::vector<std::string>& line, int mode);
    
};


// 3. Transport DATA
class speciesTrans
{
public:
    Blottner blottner;
    Sutherland sutherland;
    GeneralisedKineticTheory kinetic;
    
public:
    speciesTrans();
    ~speciesTrans();
    
    void read(const std::string& line, int mode);

    double kappa_Eucken(const double mus, const double Cvs, int mode);
    
};







class species {
public:
    std::string name;
    speciesBasic basic;
    speciesNoneq noneq;
    speciesTrans trans;
    
    
public:
    species();
    ~species();
    
public:
    double Cv(double T, int mode);
    double Cv_tra(double T);
    double Cv_rot(double T);
    double Cv_vib(double T);
    double Cv_elc(double T);
    double Cv_ele(double T);
    
    double e(double T, int mode);
    double e_tra(double T);
    double e_rot(double T);
    double e_vib(double T);
    double e_elc(double T);
    double e_ele(double T);
    

    
};


#endif /* species_hpp */
