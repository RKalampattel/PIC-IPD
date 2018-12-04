//
//  Blottner.cpp
//  OP2A
//
//  Created by Kim M.K. on 28/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "Blottner.hpp"


Blottner::Blottner()
: A(0.0), B(0.0), C(0.0)
{
    
}

Blottner::~Blottner()
{
    
}


double Blottner::mu_s(const double T)
{
    double lnT = log(T);
    double temp1 = (A*lnT + B)*lnT + C;
    
    double mu = 0.1 * exp(temp1);
    return (mu);
}