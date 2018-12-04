//
//  math_basic.cpp
//  OP2A
//
//  Created by Kim M.K. on 03/03/2017.
//  Copyright © 2017 Kim M.K. All rights reserved.
//

#include "math_basic.hpp"
#include "../COMM/error_codes.hpp"
#include "../COMM/error_exception.hpp"

double MathBasic::length(std::vector<double>& a, std::vector<double>& b)
{
    double len = 0.0;
    int nd = a.size();
    
    if (b.size() != nd)
    {
        Common::ExceptionError(FromHere(), "Dimensions of A and B do not match. BOth A and B should have same dimention", Common::ErrorCodes::Mismatch());
    }
    
    for (int k = 0; k < nd; k++) len += pow((a[k] - b[k]), 2.0);
    len = sqrt(len);
    return (len);
}
