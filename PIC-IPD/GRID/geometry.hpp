//
//  geometry.hpp
//  OP2A
//
//  Created by Kim M.K. on 09/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef geometry_hpp
#define geometry_hpp

#include <stdio.h>
#include "../MATH_MK/matrix.hpp"



// 1. Class: Common for Geometry classes
class GeometryCommon {
public:
    int  id;        // ID number (-: means allocated to other CPU/GPU, +: allocated in current CPU/GPU, 0: Not allocated yet)
    int  bcType;    // Boundary condition Type
    int  bcIndex;   // Boundary condition index
    
    int  treat;     // Flag for treatment (0: Include, 1: neet to refine, -1: need to remove)
    
    double S;       // Area/Volume (0 for point)
    matrix X;       // Location vector, X
    
    
public:
    GeometryCommon();
    ~GeometryCommon();
    
};





// 2. Class: for Node
class GeometryNode : public GeometryCommon {
public:
    
    
public:
    GeometryNode();
    ~GeometryNode();
};



// 3. Class: for Face
class GeometryFace : public GeometryCommon{
    
public:
    matrix n;               // Normal/Tangent vectors ( 0: normal vector, 1: tangential vector, 2: tangential vector)
    double distanceToWall;  // Distance to Wall;
    double nDotWall;        // n(vector) Dot Wall vector
    
public:
    GeometryFace();
    ~GeometryFace();
};



// 4. Class: for Cell
class GeometryCell : public GeometryCommon{
    
public:
    double characteristicLength;    // Characteristic Length
    double distanceToWall;          // Distance to the closest wall (or similar BCs)
    
public:
    GeometryCell();
    ~GeometryCell();
    
};


class GeometryCellPIC: public GeometryCommon{
public:
    matrix shapeFunctionInverseMatrix;
    matrix regionCoefficient;
    
public:
    GeometryCellPIC();
    ~GeometryCellPIC();
};


class GeometryCellHybrid: public GeometryCell{
public:
    matrix shapeFunctionInverseMatrix;
    matrix regionCoefficient;
    
public:
    GeometryCellHybrid();
    ~GeometryCellHybrid();
};


#endif /* geometry_hpp */
