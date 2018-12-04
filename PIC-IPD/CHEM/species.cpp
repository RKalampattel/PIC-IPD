//
//  species.cpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Last edited by Rahul Kalampattel, April 2018
//
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "species.hpp"




// 1. Basic DATA
speciesBasic::speciesBasic()
: M(0), m(0), h0(0), D(0), I(0), q(0), type(0), R(0), m_completed(false)
{
    
}

speciesBasic::~speciesBasic()
{
    
}

void speciesBasic::read(const std::string &line)
{
    sscanf_s(line.c_str(), "%lf %lf %lf %lf %lf %d", &M, &h0, &D, &I, &q, &type);
    
    m = M *AMU_SI;
    R = Ru_SI / M;
    
    Cv[0] = 1.5*R;
    if (type == 1) Cv[1] = R;
    else           Cv[1] = 0.0;
    
    m_completed = true;
}


speciesNoneq::speciesNoneq()
: Z_inf(1.0), T_star(1.0), T_ref(273.0), omega(0.74), d_ref(3.1e-10), q_rot_e(1.0), theta_v(1.0)
{
    
}

speciesNoneq::~speciesNoneq()
{
    
}


void speciesNoneq::read(const std::string& line)
{
    sscanf_s(line.c_str(), "%lf %lf %lf %lf %lf %lf %lf", &Z_inf, &T_star, &T_ref, &omega, &d_ref, &q_rot_e, &theta_v);
}


void speciesNoneq::read(const std::vector<std::string>& line, int mode)
{
    switch (mode)
    {
        case 0:
            sscanf_s(line[0].c_str(), "%lf %lf %lf %lf %lf %lf %lf", &Z_inf, &T_star, &T_ref, &omega, &d_ref, &q_rot_e, &theta_v);
            break;
            
        case 1:
            ele.read(line);
            break;
            
        case 2:
            k_ev.read(line);
            break;

    }
}


// 3. Transport DATA
speciesTrans::speciesTrans()
{
    
}

speciesTrans::~speciesTrans()
{
    
}

void speciesTrans::read(const std::string& line, int mode)
{
    switch (mode)
    {
        case 0: // Blottner
            sscanf_s(line.c_str(), "%lf %lf %lf", &blottner.A, &blottner.B, &blottner.C);
            break;
            
        case 1: // Sutherland
            sscanf_s(line.c_str(), "%lf %lf %lf %lf %lf %lf", &sutherland.S, &sutherland.T_ref, &sutherland.mu_0, &sutherland.S_kappa, &sutherland.T0_kappa, &sutherland.kappa_0);
            break;
            
        case 2: // Kinetic theory model for viscosity
            sscanf_s(line.c_str(), "%lf %lf", &kinetic.sigma, &kinetic.Teps);
            break;
    }
}


double speciesTrans::kappa_Eucken(const double mus, const double Cvs, int mode)
{
    double kappa;
    
    if (mode == 0)
    {
        kappa = 2.5 * mus * Cvs;
    }
    else
    {
        kappa = mus * Cvs;
    }
    
    return(kappa);
}





species::species()
{
    
}


species::~species()
{
    
}


double species::Cv_tra(double T)
{
    if (basic.type == -1) return (0.0);
    return (basic.Cv[0]);
}

double species::Cv_rot(double T)
{
    if (basic.type != 1) return (0.0);
    return (basic.Cv[1]);
}

double species::Cv_vib(double T)
{
    if (basic.type != 1) return (0.0);
    
    double aux1 = noneq.theta_v/T;
    double aux2 = exp(aux1);
    
    double temp = basic.R * (aux1*aux1*aux2) / pow((aux2 - 1), 2.0);
    return (temp);
}

double species::Cv_elc(double T)
{
    double aux1 = 0.0;
    double aux2 = 0.0;
    double aux3 = 0.0;
    double aux4 = 0.0;
    double temp1;
    double temp2;
    
    if (noneq.ele.lvl == 0) return (0.0);
    
    temp1 = noneq.ele.theta[0]/T;
    temp2 = noneq.ele.g[0] * exp(-temp1);
    aux1 += temp2;
    aux2 += temp1 / T * temp2;
    
    
    for (int i = 1; i < noneq.ele.lvl; i++)
    {
        temp1 = noneq.ele.theta[i]/T;
        temp2 = noneq.ele.g[i] * exp(-temp1);
        
        aux1 += temp2;
        aux2 += temp1/T * temp2;
        aux3 += temp2 * pow(temp1, 2.0);
        aux4 += temp2 * noneq.ele.theta[i];
    }
    
    double aux = basic.R * (aux3/aux1 - aux4*aux2/pow(aux1, 2.0));
    return (aux);

}


double species::Cv_ele(double T)
{
    if (basic.type != -1) return (0.0);
    else                  return (basic.Cv[0]);
}


double species::Cv(double T, int mode)
{
    double aux;
    switch (mode)
    {
        case 0:
            aux = Cv_tra(T);
            break;
            
        case 1:
            aux = Cv_rot(T);
            break;
            
        case 2:
            aux = Cv_vib(T);
            break;
            
        case 3:
            aux = Cv_elc(T);
            break;
            
        case 4:
            aux = Cv_ele(T);
            break;
    }
    
    return (aux);
}


double species::e_tra(double T)
{
    return (Cv_tra(T) * T);
}


double species::e_rot(double T)
{
    return (Cv_rot(T) * T);
}



double species::e_vib(double T)
{
    if (basic.type != 1) return (0.0);
    
    double temp;
    temp = basic.R * noneq.theta_v / (exp(noneq.theta_v/T) - 1.0);
    
    return (temp);
}

double species::e_elc(double T)
{
    double aux1 = 0.0;
    double aux2 = 0.0;
    double temp1;
    double temp2;
    
    if (noneq.ele.lvl == 0) return (0.0);
    
    aux1 += noneq.ele.g[0] * exp(-noneq.ele.theta[0]/T);
    for (int i = 1; i < noneq.ele.lvl; i++)
    {
        temp1 = noneq.ele.theta[i]/T;
        temp2 = noneq.ele.g[i] * exp(-temp1);
        
        aux1 += temp2;
        aux2 += noneq.ele.theta[i]*temp2;
    }
    
    return (basic.R  *aux2 / aux1);
}


double species::e_ele(double T)
{
    return (Cv_ele(T) *T);
}


double species::e(double T, int mode)
{
    double aux;
    switch (mode)
    {
        case 0:
            aux = e_tra(T);
            break;
            
        case 1:
            aux = e_rot(T);
            break;
            
        case 2:
            aux = e_vib(T);
            break;
            
        case 3:
            aux = e_elc(T);
            break;
            
        case 4:
            aux = e_ele(T);
            break;
    }
    
    return (aux);
}

