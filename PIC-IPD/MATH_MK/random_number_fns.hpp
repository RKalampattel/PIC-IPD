//
//  random_number_fns.hpp
//  OP2A
//
//  Created by Kim M.K. on 01/09/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef random_number_fns_hpp
#define random_number_fns_hpp

#include <stdio.h>
#include "matrix.hpp"

float* ranf_OMP(int N, int ncore);
float ranf(void);

double ranf_mkl(void);
double ranf_range(double a, double b);

matrix randi(const double imax, unsigned int m, unsigned int n);
matrix randi(const double imax, unsigned int m);



#endif /* random_number_fns_hpp */
