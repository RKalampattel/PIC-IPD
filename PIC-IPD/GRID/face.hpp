//
//  face.hpp
//  OP2A
//
//  Created by Kim M.K. on 12/01/2017.
//  Copyright © 2017 Kim M.K. All rights reserved.
//

#ifndef face_hpp
#define face_hpp

#include <stdio.h>

#include "geometry.hpp"
#include "connection.hpp"
#include "index.hpp"
#include "stencilinfo.hpp"



// 1. Base (Ancestor) Faces
class FaceBase
{
    // Part A: Data Section
public:
    GeometryFace    geometry;
    ConnectionFace  connectivity;
    
    // Part B: Constructoir / Destructor Section
public:
    FaceBase() {};
    ~FaceBase() { };
};


// 2. Face for Cartesian grid
class FaceCart : public FaceBase
{
    // Part A: Data Section
public:
    grid_index index;
    
    // Part B: Constructoir / Destructor Section
public:
    FaceCart() {};
    ~FaceCart() { };
};





#endif /* face_hpp */
