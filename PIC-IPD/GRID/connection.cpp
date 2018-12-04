//
//  connection.cpp
//  OP2A
//
//  Created by Kim M.K. on 09/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "connection.hpp"


// 1. Class: Connectiity for Nodes
ConnectionNode::ConnectionNode()
{
    sharedCellIDList.reserve(MAX_CELL_PER_NODE);
    sharedCellWeighting.reserve(MAX_CELL_PER_NODE);
};

ConnectionNode::~ConnectionNode()
{
    
};


// 2. Class: Connectivity Face
ConnectionFace::ConnectionFace()
: type(-1), numNodes(0)
{
    nodeIDs.reserve(MAX_FACE_PER_NODE);
    cr.reserve(MAX_STENCIL_INFO);
    cl.reserve(MAX_STENCIL_INFO);
    
};

ConnectionFace::~ConnectionFace()
{
    
};


// 3. Class: Connection Cell
ConnectionCell::ConnectionCell()
: type(-1), numNodes(0), numFaces(0)
{
    nodeIDs.reserve(MAX_CELL_PER_NODE);
    faceIDs.reserve(MAX_CELL_PER_FACE);
};

ConnectionCell::~ConnectionCell()
{
    
};