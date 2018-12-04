//
//  connection.hpp
//  OP2A
//
//  Created by Kim M.K. on 09/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef connection_hpp
#define connection_hpp

#include <stdio.h>
#include <vector>

#define MAX_STENCIL_INFO    2
#define MAX_FACE_PER_NODE   4
#define MAX_CELL_PER_NODE   8
#define MAX_CELL_PER_FACE   6

// 1. Class: Connectiity for Nodes
class ConnectionNode {
public:
    std::vector<int>    sharedCellIDList;
    std::vector<double>    sharedCellWeighting;
    
public:
    ConnectionNode();
    ~ConnectionNode();
    
};


// 2. Class: Connectivity Face
class ConnectionFace {
public:
    int type;       // Face element type
    int numNodes;   // Total number of nodes
    std::vector<int> nodeIDs;   // List of node ids in the face
    
    std::vector<int> cr;    // Left cell ids
    std::vector<int> cl;    // Right cell ids
    
public:
    ConnectionFace();
    ~ConnectionFace();
};


// 3. Class: Connection Cell
class ConnectionCell {
public:
    int type;       // Face element type
    int numNodes;   // Total number of nodes
    int numFaces;   // Total number of faces
    std::vector<int> nodeIDs;   // List of node ids in the cell
    std::vector<int> faceIDs;   // List of face ids in the cell
    
public:
    ConnectionCell();
    ~ConnectionCell();
};


#endif /* connection_hpp */
