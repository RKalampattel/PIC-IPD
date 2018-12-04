//
//  kev.cpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Last edited by Rahul Kalampattel, April 2018
//
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "kev.hpp"


relaxation_rate_e_v::relaxation_rate_e_v()
:lvl(0)
{
    
}

relaxation_rate_e_v::~relaxation_rate_e_v()
{
    
}


void relaxation_rate_e_v::read(const int num, const std::vector<std::string>& line)
{
    int s = 0;
    double temp1, temp2, temp3;
    
    lvl = num;
    a.resize(lvl);
    b.resize(lvl);
    c.resize(lvl);
    
    
    for (int i = 0; i < lvl; i++)
    {
        sscanf_s(line[i].c_str(), "%d %lf %lf %lf", &s, &temp1, &temp2, &temp3);
        a[s-1] = temp1;
        b[s-1] = temp2;
        c[s-1] = temp2;
    }
}


void relaxation_rate_e_v::read(const std::vector<std::string>& line)
{
    int s = 0;
    double temp1, temp2, temp3;
    
    a.resize(lvl);
    b.resize(lvl);
    c.resize(lvl);
    
    
    for (int i = 0; i < lvl; i++)
    {
        sscanf_s(line[i].c_str(), "%d %lf %lf %lf", &s, &temp1, &temp2, &temp3);
        a[s-1] = temp1;
        b[s-1] = temp2;
        c[s-1] = temp2;
    }
}
