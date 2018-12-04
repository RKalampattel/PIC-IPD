//
//  kev.hpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef kev_hpp
#define kev_hpp

#include <stdio.h>
#include <string>
#include <vector>

class relaxation_rate_e_v
{
public:
    int lvl;
    std::vector<double>    a;
    std::vector<double>    b;
    std::vector<double>    c;
    
public:
    relaxation_rate_e_v();
    ~relaxation_rate_e_v();
    void read(const int num, const std::vector<std::string>& line);
    void read(const std::vector<std::string>& line);    
};

#endif /* kev_hpp */
