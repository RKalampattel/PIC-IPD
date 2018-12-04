//
//  cell.hpp
//  OP2A
//
//  Created by Kim M.K. on 13/01/2017.
//  Copyright © 2017 Kim M.K. All rights reserved.
//

#ifndef cell_hpp
#define cell_hpp

#include <stdio.h>

#include "geometry.hpp"
#include "connection.hpp"
#include "index.hpp"
#include "stencilinfo.hpp"



// 1. Base (Ancestor) Cells
class CellBase
{
    // Part A: Data Section
public:
    GeometryCell    geometry;
    ConnectionCell  connectivity;
    
    // Part B: Constructoir / Destructor Section
public:
    CellBase()  { };
    ~CellBase() { };
};


// 2. Cell Index
class CellIndex : public grid_index
{
    // Part A: Data Section
public:
    CellBase* parent;
    std::vector<CellBase*> children;
    
    // Part B: Constructoir / Destructor Section
public:
    CellIndex();
    ~CellIndex();
};


// 2. Cell for Cartesian grid
class CellCart : public CellBase
{
    // Part A: Data Section
public:
    grid_index index;
    
    CellCart* parent;
    std::vector<CellCart*> children;
    
    // Part B: Constructoir / Destructor Section
public:
    CellCart();
    ~CellCart();
};






#endif /* cell_hpp */
