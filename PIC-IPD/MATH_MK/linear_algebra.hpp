//
//  linear_algebra.hpp
//  OP2A
//
//  Created by Kim M.K. on 31/08/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef linear_algebra_hpp
#define linear_algebra_hpp

#include <stdio.h>
#include "matrix.hpp"



// Netwon-Rapson Method
template<class F>
double newtonraphson(F f, F df, double x_initial, double tolX, double tolf, unsigned int nMAX, bool debugmode);




#endif /* linear_algebra_hpp */
