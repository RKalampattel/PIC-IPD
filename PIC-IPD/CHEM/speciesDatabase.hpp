//
//  speciesDatabase.hpp
//  OP2A
//
//  Created by Kim M.K. on 24/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef speciesDatabase_hpp
#define speciesDatabase_hpp

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <stdio.h>
#include <vector>
#include <string>

#include "../COMM/map1D.hpp"
#include "../COMM/assert_mk.hpp"
#include "../COMM/readdatafromstring.hpp"
#include "../COMM/error_codes.hpp"
#include "../COMM/error_exception.hpp"
#include "../COMM/StringOps.hpp"
#include "../COMM/VectorCompare.hpp"

#include "species.hpp"
#include "electronicState.hpp"

#define DATA_SPECIES_START  "#SPECIES_START"
#define DATA_SPECIES_END    "#SPECIES_END"



class speciesDataBase
{
//protected:
    
public:
    Common::Map1D<std::string, int> datamap;
    std::vector<species> data;

public:
    
    
    
public:
    speciesDataBase();
    ~speciesDataBase();
    
    void read(const std::string& filename);
    //void add(const std::string& filename);
    
    species find(std::string name);
};







#endif /* speciesDatabase_hpp */
