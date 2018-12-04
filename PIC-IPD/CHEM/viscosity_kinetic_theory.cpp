//
//  viscosity_kinetic_theory.cpp
//  OP2A
//
//  Created by Kim M.K. on 29/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "viscosity_kinetic_theory.hpp"

GeneralisedKineticTheory::GeneralisedKineticTheory()
: sigma(0), Teps(0)
{
    
}

GeneralisedKineticTheory::~GeneralisedKineticTheory()
{
    
}


double GeneralisedKineticTheory::omega_V(const double T)
{
    double omega;
    double T_Teps = T / Teps;
    
    omega = 1.146 * pow(T_Teps, -0.145) + pow((T_Teps + 0.5), -2.0);
    return (omega);
}


double GeneralisedKineticTheory::mu_s(const double T, const double M)
{
    double omega = omega_V(T);
    double mu;
    
    mu = 2.68e-6 * sqrt(M*T) / (sigma*sigma * omega);
    
    return (mu);
}


double GeneralisedKineticTheory::kappa_s(const double T, const double M)
{
    double omega = omega_V(T);
    
    double kappa;
    kappa = 0.0833 *sqrt(T) /(sigma*sigma*omega*sqrt(M));

    return (kappa);
}
