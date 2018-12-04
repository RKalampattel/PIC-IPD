//
//  utilities.cpp
//  OP2A
//
//  Created by Kim M.K. on 22/02/2017.
//  Copyright © 2017 Kim M.K. All rights reserved.
//

#include "utilities.hpp"
#include <cmath>




namespace Common {
    
bool Utilities::isInvalueLimits(double a, double A, double B)
{
    double left  = fmin(A, B);
    double right = fmax(A, B);
    
    if (a >= left && a <= right)    return (true);
    else                            return(false);
}
    
    
    
    
}
