//
//  utilities.hpp
//  OP2A
//
//  Created by Kim M.K. on 22/02/2017.
//  Copyright © 2017 Kim M.K. All rights reserved.
//

#ifndef utilities_hpp
#define utilities_hpp

#include <stdio.h>
#include <vector>

#include "common.hpp"
#include "noninstantiable.hpp"


namespace Common {
    
    class Utilities: public NonInstantiable<Utilities>
    {
    public:
        static bool isInvalueLimits(double a, double A, double B);

        
    };
}



#endif /* utilities_hpp */
