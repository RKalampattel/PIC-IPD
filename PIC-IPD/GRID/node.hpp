//
//  node.hpp
//  OP2A
//
//  Created by Kim M.K. on 11/01/2017.
//  Copyright © 2017 Kim M.K. All rights reserved.
//

#ifndef node_hpp
#define node_hpp

#include <stdio.h>

#include "geometry.hpp"
#include "connection.hpp"
#include "index.hpp"
#include "stencilinfo.hpp"



// 1. Base (Ancestor) Nodes
class NodeBase
{
    // Part A: Data Section
public:
    GeometryNode    geometry;
    ConnectionNode  connectivity;
    
    // Part B: Constructoir / Destructor Section
public:
    NodeBase() {};
    ~NodeBase() { };
};


// 2. Node for Cartesian grid
class NodeCart : public NodeBase
{
    // Part A: Data Section
public:
    grid_index index;
    
    // Part B: Constructoir / Destructor Section
public:
    NodeCart() {};
    ~NodeCart() { };
};









#endif /* node_hpp */
