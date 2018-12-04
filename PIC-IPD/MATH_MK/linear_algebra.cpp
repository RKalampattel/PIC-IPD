//
//  linear_algebra.cpp
//  OP2A
//
//  Created by Kim M.K. on 31/08/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//
#include <iostream>
#include "linear_algebra.hpp"



template<class F>
double newtonraphson(F f, F df, double x_initial, double tolX, double tolf, unsigned int nMAX, bool debugmode)
{
    int n = 0;
    double f_old = f(x_initial);
    double error = f_old;
    double x_old = x_initial;
    
    if (fabs(error) < tolf)
    {
        return (x_initial);
    }
    else
    {
        double x_new;
        double df_old;
        
        while (n < nMAX)
        {
            df_old = df(x_old);
            if (df_old == 0.0)
            {
                //Common::ExceptionError(FromHere(), "Newton-Rapson: df == 0", Common::ErrorCodes::ExceedLimit());
                x_old += tolX;
                df_old = df(x_old);
            }
            
            x_new = x_old - (f_old/df_old);
            
            x_old = x_new;
            f_old = f(x_old);
            error = fabs(f_old);
            if (error < tolf)
            {
                if (debugmode == true)
                {
                    std::cout << "Success to find a solution using a Newton-Raphson solver !!" << std::endl;
                    std::cout << "[Iteration] = " << n << std::endl;
                    std::cout << "[Error] = " << error << std::endl;
                }
                
                return (x_old);
            }
            else
            {
                n++;
            }
        }
    }
    
    
    Common::ExceptionError(FromHere(), "Newton-Rapson: Cannot find a solution", Common::ErrorCodes::ExceedLimit());
}
