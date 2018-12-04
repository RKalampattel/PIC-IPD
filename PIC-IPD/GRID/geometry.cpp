//
//  geometry.cpp
//  OP2A
//
//  Created by Kim M.K. on 09/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "geometry.hpp"




// 1. Class: Common for Geometry classes
//     1.1 Constructor and Destructor
GeometryCommon::GeometryCommon()
:id(0), bcType(0), bcIndex(0), treat(0), S(0.0), X(3)
{
    
};

GeometryCommon::~GeometryCommon()
{
    
};


// 2. Class: for Node
GeometryNode::GeometryNode()
{
    
};

GeometryNode::~GeometryNode()
{
    
};




// 3. Class: for Face
GeometryFace::GeometryFace()
:n(3,3), distanceToWall(0.0), nDotWall(0.0)
{
    
};

GeometryFace::~GeometryFace()
{
    
};


// 4. Class: for Cell
GeometryCell::GeometryCell()
:characteristicLength(0.0), distanceToWall(0.0)
{
    
};

GeometryCell::~GeometryCell()
{
    
};


GeometryCellPIC::GeometryCellPIC()
{
    
};

GeometryCellPIC::~GeometryCellPIC()
{
    
};

GeometryCellHybrid::GeometryCellHybrid()
{
    
};

GeometryCellHybrid::~GeometryCellHybrid()
{
    
};