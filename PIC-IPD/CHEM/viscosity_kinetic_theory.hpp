//
//  viscosity_kinetic_theory.hpp
//  OP2A
//
//  Created by Kim M.K. on 29/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef viscosity_kinetic_theory_hpp
#define viscosity_kinetic_theory_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>


class GeneralisedKineticTheory {
public:
    double sigma;   // collision diameter, A0
    double Teps;    // Effective temperature characteristic
    
public:
    GeneralisedKineticTheory();
    ~GeneralisedKineticTheory();
    
    
    double omega_V(const double T);
    double mu_s(const double T, const double m);
    double kappa_s(const double T, const double m);
};

#endif /* viscosity_kinetic_theory_hpp */
