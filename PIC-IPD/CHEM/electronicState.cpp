//
//  electronicState.cpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Last edited by Rahul Kalampattel, April 2018
//
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "electronicState.hpp"


electronicState::electronicState()
:lvl(0)
{
    theta.reserve(20);
    g.reserve(20);
}


electronicState::~electronicState()
{
    
}


void electronicState::read(const int num, const std::vector<std::string>& line)
{
    int s = 0;
    double temp1, temp2;
    
    lvl = num;
    theta.resize(lvl);
    g.resize(lvl);
    
    for (int i = 0; i < lvl; i++)
    {
        sscanf_s(line[i].c_str(), "%d %lf %lf", &s, &temp1, &temp2);
        theta[s-1] = temp1;
        g[s-1] = temp2;
    }
}


void electronicState::read(const std::vector<std::string>& line)
{
    int s = 0;
    double temp1, temp2;

    theta.resize(lvl);
    g.resize(lvl);
    
    for (int i = 0; i < lvl; i++)
    {
        sscanf_s(line[i].c_str(), "%d %lf %lf", &s, &temp1, &temp2);
        
        if (s > lvl)
        {
            theta.resize(s+1);
            g.resize(s+1);
        }
        
        theta[s-1] = temp1;
        g[s-1] = temp2;
    }
}
