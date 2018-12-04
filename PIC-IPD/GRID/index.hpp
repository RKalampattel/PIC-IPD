//
//  index.hpp
//  OP2A
//
//  Created by Kim M.K. on 08/09/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef index_hpp
#define index_hpp

#include <stdio.h>
#include <vector>
#include "../COMM/MultiVector.hpp"

#define MAX_REFINE_LVL  5


// 1. Class for Index ver1
class grid_index {

//      [Part A]: section for Data
public:
    int i;
    int j;
    int k;
    
    unsigned int lvl;
    std::vector<unsigned int> pos;
    
//      [Part B]: Section constructor and destrutor
public:
    grid_index();
    ~grid_index();
    
//      [Part C]: Section for Funtions
public:
    bool hasChildren;
    void refine(unsigned int position);
};


// 2. Class for Index Ver 2
class grid_index_ver2 : public grid_index
{
public:
    int parent;
    std::vector<int> children;
    
public:
    grid_index_ver2();
    ~grid_index_ver2();
};




// 3. Class for global index
template <class T>
class global_index {
public:
    global_index()
    : Nx(0), Ny(0), Nz(0), Num(0)
    {
        
    };
    
    
    ~global_index()
    {
        
    };
    
public:
    unsigned int Nx;
    unsigned int Ny;
    unsigned int Nz;
    unsigned int Num;
    
    intvector3D ikj;                // ID of main data
    std::vector<T* > id_to_ptr;
    
public:
    void add(unsigned int id, T* data)
    {
        int m = id_to_ptr.size();
        
        if (m < id +1)
        {
            id_to_ptr.resize(id +1);
        }
        
        id_to_ptr[id] = data;
    };
    
    
    void del(unsigned int id)
    {
        int m = id_to_ptr.size();
        
        if (id <= m-1)
        {
            id_to_ptr[id] = NULL;
        }
    };
    
};







#endif /* index_hpp */
