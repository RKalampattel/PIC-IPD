//
//  Blottner.hpp
//  OP2A
//
//  Created by Kim M.K. on 28/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef Blottner_hpp
#define Blottner_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

class Blottner {
public:
    double A;
    double B;
    double C;
    
public:
    Blottner();
    ~Blottner();
    
    double mu_s(const double T);
};




#endif /* Blottner_hpp */
