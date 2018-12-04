//
//  LeRc.hpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef LeRc_hpp
#define LeRc_hpp

#include <stdio.h>
#include <string>
#include <vector>



class LeRc {
public:
    int lvl;
    std::vector<double> hf;
    std::vector<double> T1;
    std::vector<double> Tu;
    std::vector<double> a1;
    std::vector<double> a2;
    std::vector<double> a3;
    std::vector<double> a4;
    std::vector<double> a5;
    std::vector<double> a6;
    std::vector<double> a7;
    std::vector<double> a8;
    std::vector<double> b1;
    std::vector<double> b2;
    
public:
    LeRc();
    ~LeRc();
    
    void read(const int num, const std::vector<std::string>& line);
    void read(const std::vector<std::string>& line);
    
    double Cp_over_R(const double T);
    double Cv_over_R(const double T);
    double h0_over_RT(const double T);
    double e_over_RT(const double T);
    double s_over_R(const double T);
    
    int findRange(const double T);
    
    
    
};


#endif /* LeRc_hpp */
