//
//  Sutherland.hpp
//  OP2A
//
//  Created by Kim M.K. on 28/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef Sutherland_hpp
#define Sutherland_hpp


#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>


class Sutherland {
public:
    double S;
    double mu_0;
    double T_ref;
    
    double T0_kappa;
    double kappa_0;
    double S_kappa;
    
    
public:
    Sutherland();
    ~Sutherland();
    
    double mu_s(const double T);
    double kappa_s(const double T);

};


#endif /* Sutherland_hpp */
