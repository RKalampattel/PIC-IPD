//
//  LeRc.cpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Last edited by Rahul Kalampattel, April 2018
//
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "LeRc.hpp"
#include <cmath>



LeRc::LeRc()
:lvl(0)
{
    
}


LeRc::~LeRc()
{
    
}


void LeRc::read(const int num, const std::vector<std::string>& line)
{
    int s = 0;
    double temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9;
    double tempT1, tempTu, temphf;
    
    lvl = num;
    hf.resize(lvl+2);
    T1.resize(lvl+2);
    Tu.resize(lvl+2);
    a1.resize(lvl+2);
    a2.resize(lvl+2);
    a3.resize(lvl+2);
    a4.resize(lvl+2);
    a5.resize(lvl+2);
    a6.resize(lvl+2);
    a7.resize(lvl+2);
    a8.resize(lvl+2);
    b1.resize(lvl+2);
    b2.resize(lvl+2);
    
    for (int i = 1; i <= lvl; i++)
    {
        sscanf_s(line[i-1].c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
               &hf[i], &T1[i], &Tu[i], &a1[i], &a2[i], &a3[i], &a4[i], &a5[i], &a6[i], &a7[i], &a8[i], &b1[i], &b2[i]);
    }
    
    // Extend the coefficient bottom and upper limit
    double Cp0 = Cp_over_R(T1[1]);
    double h0  = h0_over_RT(T1[1]);
    double s0  = s_over_R(T1[1]);
    
    T1[0] = 0.0;
    Tu[0] = T1[1];
    a1[0] = 0.0;
    a2[0] = 0.0;
    a3[0] = Cp0;
    a4[0] = 0.0;
    a5[0] = 0.0;
    a6[0] = 0.0;
    a7[0] = 0.0;
    a8[0] = 0.0;
    b1[0] = T1[1] * (h0 - a3[0]);
    b2[0] = s0 - a3[0]*log(T1[1]);
    
    
    double T = Tu[lvl];
    double Cp_u = Cp_over_R(T);
    double h_u  = h0_over_RT(T);
    double s_u  = s_over_R(T);
    
    T1[lvl+1] = T;
    Tu[lvl+1] = 1.0e10;
    a1[lvl+1] = 0.0;
    a2[lvl+1] = 0.0;
    a3[lvl+1] = Cp_u;
    a4[lvl+1] = 0.0;
    a5[lvl+1] = 0.0;
    a6[lvl+1] = 0.0;
    a7[lvl+1] = 0.0;
    a8[lvl+1] = 0.0;
    b1[lvl+1] = T * (h_u - a3[lvl+1]);
    b2[lvl+1] = s_u - a3[lvl+1]*log(T);
   
    lvl =  lvl;
}


void LeRc::read(const std::vector<std::string>& line)
{
    int s = 0;
    double temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9;
    double tempT1, tempTu, temphf;
    
    hf.resize(lvl+2);
    T1.resize(lvl+2);
    Tu.resize(lvl+2);
    a1.resize(lvl+2);
    a2.resize(lvl+2);
    a3.resize(lvl+2);
    a4.resize(lvl+2);
    a5.resize(lvl+2);
    a6.resize(lvl+2);
    a7.resize(lvl+2);
    a8.resize(lvl+2);
    b1.resize(lvl+2);
    b2.resize(lvl+2);
    
    
    for (int i = 1; i <= lvl; i++)
    {
        sscanf_s(line[i-1].c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
               &hf[i], &T1[i], &Tu[i], &a1[i], &a2[i], &a3[i], &a4[i], &a5[i], &a6[i], &a7[i], &a8[i], &b1[i], &b2[i]);
    }
    
    
    // Extend the coefficient bottom and upper limit
    double Cp0 = Cp_over_R(T1[1]);
    double h0  = h0_over_RT(T1[1]);
    double s0  = s_over_R(T1[1]);
    
    T1[0] = 0.0;
    Tu[0] = T1[1];
    a1[0] = 0.0;
    a2[0] = 0.0;
    a3[0] = Cp0;
    a4[0] = 0.0;
    a5[0] = 0.0;
    a6[0] = 0.0;
    a7[0] = 0.0;
    a8[0] = 0.0;
    b1[0] = T1[1] * (h0 - a3[0]);
    b2[0] = s0 - a3[0]*log(T1[1]);
    
    
    double T = Tu[lvl];
    double Cp_u = Cp_over_R(T);
    double h_u  = h0_over_RT(T);
    double s_u  = s_over_R(T);

    T1[lvl+1] = T;
    Tu[lvl+1] = 1.0e10;
    a1[lvl+1] = 0.0;
    a2[lvl+1] = 0.0;
    a3[lvl+1] = Cp_u;
    a4[lvl+1] = 0.0;
    a5[lvl+1] = 0.0;
    a6[lvl+1] = 0.0;
    a7[lvl+1] = 0.0;
    a8[lvl+1] = 0.0;
    b1[lvl+1] = T * (h_u - a3[lvl+1]);
    b2[lvl+1] = s_u - a3[lvl+1]*log(T);
    
    lvl =  lvl;
}




int LeRc::findRange(const double T)
{
    if (T < T1[1])    return (0);
    if (T > Tu[lvl])  return (lvl+1);

    for (int i = 1; i <= lvl; i++)
    {
        if (T >= T1[i] && T < Tu[i])
        {
            return (i);
        }
    }
    
    
    return (0);
}

double LeRc::Cp_over_R(const double T)
{
    double Cp_o_R = 0.0;
    int i = findRange (T);
    
    Cp_o_R = a1[i]/T/T + a2[i]/T + a3[i] + a4[i]*T + a5[i]*T*T + a6[i]*T*T*T + a7[i]*T*T*T*T;
    return (Cp_o_R);
}

double LeRc::Cv_over_R(const double T)
{
    return (Cp_over_R(T) - 1.0);
}


double LeRc::h0_over_RT(const double T)
{
    int i = findRange (T);
    double h0_o_RT = 0.0;
    h0_o_RT = -a1[i]/T/T + a2[i]*log(T)/T + a3[i] + a4[i]*T/2.0 + a5[i]*T*T/3.0 + a6[i]*T*T*T/4.0 + a7[i]*T*T*T*T/5.0 + a8[i] + b1[i]/T;
        
    return (h0_o_RT);
}

double LeRc::e_over_RT(const double T)
{
    return (h0_over_RT(T) - 1.0);
}


double LeRc::s_over_R(const double T)
{
    int i = findRange (T);
    double s_o_R = 0.0;
    
    s_o_R = -a1[i]/(2.0*T*T) - a2[i]/T + a3[i]*log(T) + a4[i]*T + a5[i]*T*T/2.0 + a6[i]*T*T*T/3.0 + a7[i]*T*T*T*T/4.0 +a8[i]*log(T) + b2[i];
    
    return (s_o_R);

}




































