//
//  math_basic.hpp
//  OP2A
//
//  Created by Kim M.K. on 03/03/2017.
//  Copyright © 2017 Kim M.K. All rights reserved.
//

#ifndef math_basic_hpp
#define math_basic_hpp

#include <stdio.h>
#include <vector>

#include "../COMM/common.hpp"
#include "../COMM/noninstantiable.hpp"



class MathBasic: public Common::NonInstantiable<MathBasic>
{
public:
    static double length(std::vector<double>& a, std::vector<double>& b);
        
        
};


#endif /* math_basic_hpp */
