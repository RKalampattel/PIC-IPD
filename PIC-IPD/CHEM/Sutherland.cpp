//
//  Sutherland.cpp
//  OP2A
//
//  Created by Kim M.K. on 28/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "Sutherland.hpp"


Sutherland::Sutherland()
: S(0), mu_0(0), T_ref(0)
{
    
}

Sutherland::~Sutherland()
{
    
}


double Sutherland::mu_s(const double T)
{
    double mu;
    double temp;
    
    temp = (T_ref + S) / (T + S);
    mu   = mu_0 * pow(T/T_ref, 1.5) * temp;
    
    return (mu);
}


double Sutherland::kappa_s(const double T)
{
    double temp1 = (T0_kappa + S_kappa) / (T + S_kappa);
    double kappa;
    
    kappa = kappa_0 * pow(T/T0_kappa, 1.5) * temp1;
    return (kappa);
}