//
//  electronicState.hpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef electronicState_hpp
#define electronicState_hpp

#include <stdio.h>
#include <string>
#include <vector>


class electronicState {
public:
    int lvl;                    // Number of electronic state level
    std::vector<double> theta;  // Characteristric temp [K]
    std::vector<int>    g;      // degeneracy
    
public:
    electronicState();
    ~electronicState();
    
    void read(const int num, const std::vector<std::string>& line);
    void read(const std::vector<std::string>& line);

};


#endif /* electronicState_hpp */
