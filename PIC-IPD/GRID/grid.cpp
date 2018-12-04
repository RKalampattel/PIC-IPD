//
//  grid.cpp
//  OP2A
//
//  Created by Kim M.K. on 10/11/2016.
//  Last edited by Rahul Kalampattel, April 2018
//  
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include "grid.hpp"

///////////////////////
// Classes for Grid
///////////////////////
// 1. Class for BASIC INFO
//      Part B: Constructoir / Destructor Section
GridBasicInfo::GridBasicInfo()
:gridFactor(0.0), isAxisymmeric(0), DIM(0), NNM(0), NFM(0), NCM(0), NGM(0), m_completed(false)
{
    
}

GridBasicInfo::~GridBasicInfo()
{
    
}

// Part C: Functions
bool GridBasicInfo::isCompleted()
{
    return (m_completed);
}

void GridBasicInfo::setComplete()
{
    m_completed = true;
}

void GridBasicInfo::needToUpdate()
{
    m_completed = false;
}

void GridBasicInfo::read(const std::string& mesh_file_name)
{
    // 1. Open file to read
    std::ifstream mesh_file;
    mesh_file.open(mesh_file_name.c_str());
    if (!mesh_file.is_open())
    {
        Common::ExceptionError(FromHere(), "Cannot find grid file. Please check your grid file!", Common::ErrorCodes::FileSystem());
    }
    
    DIM = -1;
    NNM = -1;
    NFM = -1;
    NCM = -1;
    NGM = -1;
        
    
    int tempInt;
    std::string line;
        
        
    // Over information read
    while (!mesh_file.eof())
    {
        getline(mesh_file, line);
            
        tempInt = -1;
        tempInt = Common::read_data_from_string::read_numeric<int>(line, "DIM= ");
        if (tempInt != -1)	DIM = tempInt;
            
        tempInt = -1;
        tempInt = Common::read_data_from_string::read_numeric<int>(line, "NNM= ");
        if (tempInt != -1)	NNM = tempInt;
            
        tempInt = -1;
        tempInt = Common::read_data_from_string::read_numeric<int>(line, "NFM= ");
        if (tempInt != -1)	NFM = tempInt;
        
        tempInt = -1;
        tempInt = Common::read_data_from_string::read_numeric<int>(line, "NCM= ");
        if (tempInt != -1)	NCM = tempInt;
    }
        
    if (DIM == -1) Common::ExceptionError(FromHere(), "Cannot find dimension information in the gridfile!", Common::ErrorCodes::NoSuchValue());
    if (NNM == -1) Common::ExceptionError(FromHere(), "Cannot find NNM information in the grid file!", Common::ErrorCodes::NoSuchValue());
    if (NFM == -1) Common::ExceptionError(FromHere(), "Cannot find NFM information in the grid file!", Common::ErrorCodes::NoSuchValue());
    if (NCM == -1) Common::ExceptionError(FromHere(), "Cannot find NCM information in the grid file!", Common::ErrorCodes::NoSuchValue());
    
    m_completed = true;
}




// 2. Class for MPI Data
//      Part B: Constructoir / Destructor Section
GridMPI::GridMPI()
: numLines(0), sendGhost(NULL), receiveGhost(NULL)
{
    
}

GridMPI::~GridMPI()
{
    sendGhost = NULL;
    receiveGhost = NULL;
}





// 3. Base class for Grid-class (Ancestor)
//      [Part B]: Constructor / Destructor section
GridGeo::GridGeo()
{
    
}

GridGeo::GridGeo(GridBasicInfo& gridInfo)
: nodes(gridInfo.NNM), faces(gridInfo.NFM), cells(gridInfo.NCM)
{
    
}

GridGeo::~GridGeo()
{
    
}

//      [Part C]: Functions
NodeBase& GridGeo::Node(int id)
{
    op_assert(id > 0);
    int npos = whereisNodes[id];
    return (nodes[npos]);
}

FaceBase& GridGeo::Face(int id)
{
    op_assert(id > 0);
    int npos = whereisFaces[id];
    return (faces[npos]);
}

CellBase& GridGeo::Cell(int id)
{
    int pos;
    
    if (id < 0)
    {
        pos = whereisGhost[-id];
        return (ghost[pos]);
    }
    else
    {
        pos = whereisCells[id];
        return (cells[pos]);
    }
}

void GridGeo::readGrid(const std::string& mesh_file_name, GridBasicInfo& gridInfo)
{
    // 1. Open file to read
    if (gridInfo.isCompleted() != true)   gridInfo.read(mesh_file_name);
    
    // Set the size of data and initialize mapping data
    nodes.resize(gridInfo.NNM);	//grid.nodeIDMap.clear();
    faces.resize(gridInfo.NFM);	//grid.faceIDMap.clear();
    cells.resize(gridInfo.NCM);	//grid.cellIDMap.clear();
    whereisNodes.resize(gridInfo.NNM+1);
    whereisFaces.resize(gridInfo.NFM+1);
    whereisCells.resize(gridInfo.NCM+1);
    
    
    int tempInt;
    std::string line;
    std::ifstream mesh_file;
    mesh_file.open(mesh_file_name.c_str());
    
    // Read Node Data
    while (!mesh_file.eof())
    {
        getline(mesh_file, line);
        
        tempInt = -1;
        tempInt = Common::read_data_from_string::read_numeric<int>(line, "NODEDATA= ");
        if (tempInt != -1)
        {
            if (tempInt != gridInfo.NNM) Common::ExceptionError(FromHere(), "Mismatch the number of node data!", Common::ErrorCodes::MismatchData());
            
            for (int n = 0; n <= gridInfo.NNM-1; n++)
            {
                int nodeID = -1;
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;
                
                getline(mesh_file, line);
                sscanf_s(line.c_str(), "%d %lf %lf %lf", &nodeID, &x, &y, &z);
                
                if (nodeID != -1)
                {
                    nodes[n].geometry.id = nodeID;
                    nodes[n].geometry.X(0) = x;
                    nodes[n].geometry.X(1) = y;
                    nodes[n].geometry.X(2) = z;
                    
                    if (nodeID + 1 > whereisNodes.size()) whereisNodes.resize(nodeID+1);
                    whereisNodes[nodeID] = n;
                }
            }
        }
    }
    
    
    // Read Face Data
    mesh_file.clear();
    mesh_file.seekg(0, std::ios::beg);
    while (!mesh_file.eof())
    {
        getline(mesh_file, line);
        
        tempInt = -1;
        tempInt = Common::read_data_from_string::read_numeric<int>(line, "FACEDATA= ");
        if (tempInt != -1)
        {
            if (tempInt != gridInfo.NFM)
            {
                Common::ExceptionError(FromHere(), "Mismatch the number of face data!", Common::ErrorCodes::MismatchData());
            }
            
            for (int f = 0; f <= gridInfo.NFM-1; f++)
            {
                int faceID = -1;
                int faceBC = -1;
                int faceType = -1;
                int cl = -1;
                int cr = -1;
                std::vector<int> faceNodeList(MAX_FACE_PER_NODE, -1);
                
                getline(mesh_file, line);
                sscanf_s(line.c_str(), "%d %d %d", &faceID, &faceBC, &faceType);
                
                
                
                if (faceID != -1)
                {
                    faces[f].geometry.id       = faceID;
                    faces[f].geometry.bcType   = faceBC;
                    faces[f].connectivity.type = faceType;
                    
                    
                    switch(faceType)
                    {
                        case LINE:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d", &faceID, &faceBC, &faceType, &faceNodeList[0], &faceNodeList[1], &cl, &cr);
                            
                            faces[f].connectivity.numNodes   = 2;
                            faces[f].connectivity.nodeIDs.resize(faces[f].connectivity.numNodes);
                            faces[f].connectivity.nodeIDs[0] = faceNodeList[0];
                            faces[f].connectivity.nodeIDs[1] = faceNodeList[1];
                            break;
                            
                        case TRI3:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d", &faceID, &faceBC, &faceType, &faceNodeList[0], &faceNodeList[1], &faceNodeList[2], &cl, &cr);
                            
                            faces[f].connectivity.numNodes   = 3;
                            faces[f].connectivity.nodeIDs.resize(faces[f].connectivity.numNodes);
                            faces[f].connectivity.nodeIDs[0] = faceNodeList[0];
                            faces[f].connectivity.nodeIDs[1] = faceNodeList[1];
                            faces[f].connectivity.nodeIDs[2] = faceNodeList[2];
                            break;
                            
                        case QUAD4:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d", &faceID, &faceBC, &faceType, &faceNodeList[0], &faceNodeList[1], &faceNodeList[2], &faceNodeList[3], &cl, &cr);
                            
                            faces[f].connectivity.numNodes   = 4;
                            faces[f].connectivity.nodeIDs.resize(faces[f].connectivity.numNodes);
                            faces[f].connectivity.nodeIDs[0] = faceNodeList[0];
                            faces[f].connectivity.nodeIDs[1] = faceNodeList[1];
                            faces[f].connectivity.nodeIDs[2] = faceNodeList[2];
                            faces[f].connectivity.nodeIDs[3] = faceNodeList[3];
                            break;
                            
                        default:
                            Common::ExceptionError(FromHere(), "It is not supported face-type", Common::ErrorCodes::NotSupportedType());
                            break;
                    }
                    
                    faces[f].connectivity.cl.resize(1);
                    faces[f].connectivity.cr.resize(1);
                    faces[f].connectivity.cl[0] = cl;
                    faces[f].connectivity.cr[0] = cr;
                    
                    if (faceID + 1 > whereisFaces.size()) whereisFaces.resize(faceID+1);
                    whereisFaces[faceID] = f;
                }
            }
            
        }
    }
    
    
    
    // Read Cell Data
    mesh_file.clear();
    mesh_file.seekg(0, std::ios::beg);
    while (!mesh_file.eof())
    {
        getline(mesh_file, line);
        
        tempInt = -1;
        tempInt = Common::read_data_from_string::read_numeric<int>(line, "CELLDATA= ");
        if (tempInt != -1)
        {
            if (tempInt != gridInfo.NCM) Common::ExceptionError(FromHere(), "Mismatch the number of cell data!", Common::ErrorCodes::MismatchData());
            
            for (int c = 0; c <= gridInfo.NCM-1; c++)
            {
                int cellID   = -1;
                int cellBC   = -1;
                int cellType = -1;
                std::vector<int> cellNodeList(MAX_CELL_PER_NODE, -1);
                std::vector<int> cellFaceList(MAX_CELL_PER_FACE, -1);
                
                getline(mesh_file, line);
                sscanf_s(line.c_str(), "%d %d %d", &cellID, &cellBC, &cellType);
                
                if (cellID != -1)
                {
                    cells[c].geometry.id       = cellID;
                    cells[c].geometry.bcType   = cellBC;
                    cells[c].connectivity.type = cellType;
                    
                    
                    switch(cellType)
                    {
                        case TRI3:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d", &cellID, &cellBC, &cellType, &cellNodeList[0], &cellNodeList[1], &cellNodeList[2], &cellFaceList[0], &cellFaceList[1], &cellFaceList[2]);
                            
                            cells[c].connectivity.numNodes   = 3;
                            cells[c].connectivity.nodeIDs.resize(cells[c].connectivity.numNodes);
                            cells[c].connectivity.nodeIDs[0] = cellNodeList[0];
                            cells[c].connectivity.nodeIDs[1] = cellNodeList[1];
                            cells[c].connectivity.nodeIDs[2] = cellNodeList[2];
                            
                            cells[c].connectivity.numFaces   = 3;
                            cells[c].connectivity.faceIDs.resize(cells[c].connectivity.numFaces);
                            cells[c].connectivity.faceIDs[0] = cellFaceList[0];
                            cells[c].connectivity.faceIDs[1] = cellFaceList[1];
                            cells[c].connectivity.faceIDs[2] = cellFaceList[2];
                            break;
                            
                        case QUAD4:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d %d %d", &cellID, &cellBC, &cellType, &cellNodeList[0], &cellNodeList[1], &cellNodeList[2],  &cellNodeList[3], &cellFaceList[0], &cellFaceList[1], &cellFaceList[2],  &cellFaceList[3]);
                            
                            cells[c].connectivity.numNodes   = 4;
                            cells[c].connectivity.nodeIDs.resize(cells[c].connectivity.numNodes);
                            cells[c].connectivity.nodeIDs[0] = cellNodeList[0];
                            cells[c].connectivity.nodeIDs[1] = cellNodeList[1];
                            cells[c].connectivity.nodeIDs[2] = cellNodeList[2];
                            cells[c].connectivity.nodeIDs[3] = cellNodeList[3];
                            
                            cells[c].connectivity.numFaces   = 4;
                            cells[c].connectivity.faceIDs.resize(cells[c].connectivity.numFaces);
                            cells[c].connectivity.faceIDs[0] = cellFaceList[0];
                            cells[c].connectivity.faceIDs[1] = cellFaceList[1];
                            cells[c].connectivity.faceIDs[2] = cellFaceList[2];
                            cells[c].connectivity.faceIDs[3] = cellFaceList[3];
                            break;
                            
                        case TETRA4:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d %d %d", &cellID, &cellBC, &cellType, &cellNodeList[0], &cellNodeList[1], &cellNodeList[2],  &cellNodeList[3], &cellFaceList[0], &cellFaceList[1], &cellFaceList[2],  &cellFaceList[3]);
                            
                            cells[c].connectivity.numNodes   = 4;
                            cells[c].connectivity.nodeIDs.resize(cells[c].connectivity.numNodes);
                            cells[c].connectivity.nodeIDs[0] = cellNodeList[0];
                            cells[c].connectivity.nodeIDs[1] = cellNodeList[1];
                            cells[c].connectivity.nodeIDs[2] = cellNodeList[2];
                            cells[c].connectivity.nodeIDs[3] = cellNodeList[3];
                            
                            cells[c].connectivity.numFaces   = 4;
                            cells[c].connectivity.faceIDs.resize(cells[c].connectivity.numFaces);
                            cells[c].connectivity.faceIDs[0] = cellFaceList[0];
                            cells[c].connectivity.faceIDs[1] = cellFaceList[1];
                            cells[c].connectivity.faceIDs[2] = cellFaceList[2];
                            cells[c].connectivity.faceIDs[3] = cellFaceList[3];
                            break;
                            
                        case HEXA8:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &cellID, &cellBC, &cellType,
                                   &cellNodeList[0], &cellNodeList[1], &cellNodeList[2], &cellNodeList[3], &cellNodeList[4], &cellNodeList[5], &cellNodeList[6], &cellNodeList[7],
                                   &cellFaceList[0], &cellFaceList[1], &cellFaceList[2], &cellFaceList[3], &cellFaceList[4], &cellFaceList[5]);
                            
                            cells[c].connectivity.numNodes   = 8;
                            cells[c].connectivity.nodeIDs.resize(cells[c].connectivity.numNodes);
                            cells[c].connectivity.nodeIDs[0] = cellNodeList[0];
                            cells[c].connectivity.nodeIDs[1] = cellNodeList[1];
                            cells[c].connectivity.nodeIDs[2] = cellNodeList[2];
                            cells[c].connectivity.nodeIDs[3] = cellNodeList[3];
                            cells[c].connectivity.nodeIDs[4] = cellNodeList[4];
                            cells[c].connectivity.nodeIDs[5] = cellNodeList[5];
                            cells[c].connectivity.nodeIDs[6] = cellNodeList[6];
                            cells[c].connectivity.nodeIDs[7] = cellNodeList[7];
                            
                            cells[c].connectivity.numFaces   = 6;
                            cells[c].connectivity.faceIDs.resize(cells[c].connectivity.numFaces);
                            cells[c].connectivity.faceIDs[0] = cellFaceList[0];
                            cells[c].connectivity.faceIDs[1] = cellFaceList[1];
                            cells[c].connectivity.faceIDs[2] = cellFaceList[2];
                            cells[c].connectivity.faceIDs[3] = cellFaceList[3];
                            cells[c].connectivity.faceIDs[4] = cellFaceList[4];
                            cells[c].connectivity.faceIDs[5] = cellFaceList[5];
                            
                            break;
                            
                        case PRISM6:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d %d %d %d %d %d", &cellID, &cellBC, &cellType,
                                   &cellNodeList[0], &cellNodeList[1], &cellNodeList[2], &cellNodeList[3], &cellNodeList[4], &cellNodeList[5],
                                   &cellFaceList[0], &cellFaceList[1], &cellFaceList[2], &cellFaceList[3], &cellFaceList[4]);
                            
                            cells[c].connectivity.numNodes   = 6;
                            cells[c].connectivity.nodeIDs.resize(cells[c].connectivity.numNodes);
                            cells[c].connectivity.nodeIDs[0] = cellNodeList[0];
                            cells[c].connectivity.nodeIDs[1] = cellNodeList[1];
                            cells[c].connectivity.nodeIDs[2] = cellNodeList[2];
                            cells[c].connectivity.nodeIDs[3] = cellNodeList[3];
                            cells[c].connectivity.nodeIDs[4] = cellNodeList[4];
                            cells[c].connectivity.nodeIDs[5] = cellNodeList[5];
                            
                            cells[c].connectivity.numFaces   = 5;
                            cells[c].connectivity.faceIDs.resize(cells[c].connectivity.numFaces);
                            cells[c].connectivity.faceIDs[0] = cellFaceList[0];
                            cells[c].connectivity.faceIDs[1] = cellFaceList[1];
                            cells[c].connectivity.faceIDs[2] = cellFaceList[2];
                            cells[c].connectivity.faceIDs[3] = cellFaceList[3];
                            cells[c].connectivity.faceIDs[4] = cellFaceList[4];
                            break;
                            
                        case PYRAMID5:
                            sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d %d %d %d %d", &cellID, &cellBC, &cellType,
                                   &cellNodeList[0], &cellNodeList[1], &cellNodeList[2], &cellNodeList[3], &cellNodeList[4],
                                   &cellFaceList[0], &cellFaceList[1], &cellFaceList[2], &cellFaceList[3], &cellFaceList[4]);
                            
                            cells[c].connectivity.numNodes   = 5;
                            cells[c].connectivity.nodeIDs.resize(cells[c].connectivity.numNodes);
                            cells[c].connectivity.nodeIDs[0] = cellNodeList[0];
                            cells[c].connectivity.nodeIDs[1] = cellNodeList[1];
                            cells[c].connectivity.nodeIDs[2] = cellNodeList[2];
                            cells[c].connectivity.nodeIDs[3] = cellNodeList[3];
                            cells[c].connectivity.nodeIDs[4] = cellNodeList[4];
                            
                            cells[c].connectivity.numFaces   = 5;
                            cells[c].connectivity.faceIDs.resize(cells[c].connectivity.numFaces);
                            cells[c].connectivity.faceIDs[0] = cellFaceList[0];
                            cells[c].connectivity.faceIDs[1] = cellFaceList[1];
                            cells[c].connectivity.faceIDs[2] = cellFaceList[2];
                            cells[c].connectivity.faceIDs[3] = cellFaceList[3];
                            cells[c].connectivity.faceIDs[4] = cellFaceList[4];
                            break;
                            
                        default:
                            Common::ExceptionError(FromHere(), "It is not supported cell-type", Common::ErrorCodes::NotSupportedType());
                            break;
                    }
                    
                    if (cellID + 1 > whereisCells.size()) whereisCells.resize(cellID+1);
                    whereisCells[cellID] = c;
                }
            }
        }
    }
    
    mesh_file.close();
}



//////////////////////
// Grid Utilities
//////////////////////
void readGridFromFile(const std::string& mesh_file_name, GridBasicInfo& gridinfo, GridGeo& gridgeo)
{
    gridinfo.read(mesh_file_name);
    gridgeo.readGrid(mesh_file_name, gridinfo);
}


void processingGrid(GridBasicInfo& gridinfo, GridGeo& gridgeo)
{
    // 1 Processing for Nodes
    if (gridinfo.gridFactor == 0.0)
    {
        gridinfo.gridFactor = 1.0;
    }
    else if (gridinfo.gridFactor != 1.0)
    {
#pragma ivdep
        for (int n = 0; n < gridinfo.NNM; n++)
        {
            for (int k = 0; k < gridinfo.DIM; k++)
            {
                gridgeo.nodes[n].geometry.X(k) /= gridinfo.gridFactor;
            }
        }
        
    }
    
    if (gridinfo.isAxisymmeric == true && gridinfo.DIM == 2)
    {
#pragma ivdep
        for (int n = 0; n < gridinfo.NNM; n++)
        {
            gridgeo.nodes[n].geometry.X(1) = fabs(gridgeo.nodes[n].geometry.X(1)) + GRID_AXISMEMTRIC_Y;
        }
    }
    else if (gridinfo.DIM == 3)
    {
        gridinfo.isAxisymmeric = false;
    }
    
    
    // 2. Processing for Faces
#pragma ivdep
    for (int f = 0; f < gridinfo.NFM; f++)
    {
        matrix x1(3);
        matrix x2(3);
        matrix x3(3);
        
        NodeBase* n1 = &gridgeo.Node(gridgeo.faces[f].connectivity.nodeIDs[0]);
        NodeBase* n2 = &gridgeo.Node(gridgeo.faces[f].connectivity.nodeIDs[1]);
        NodeBase* n3 = NULL;
        NodeBase* n4 = NULL;
        
        switch (gridgeo.faces[f].connectivity.type)
        {
            case LINE:
                gridgeo.faces[f].geometry.S = area_line(n1->geometry.X, n2->geometry.X);
                
                // Tangential vector
                x1 = vectorStartEnd(n1->geometry.X, n2->geometry.X);
                x1.normalize();
                for (int d = 0; d < 3; d++) gridgeo.faces[f].geometry.n(GRID_TAN1, d) = x1(d);
                
                // Normal Vector
                gridgeo.faces[f].geometry.n(GRID_NOR, 0) =  gridgeo.faces[f].geometry.n(GRID_TAN1, 1);
                gridgeo.faces[f].geometry.n(GRID_NOR, 1) = -gridgeo.faces[f].geometry.n(GRID_TAN1, 0);
                gridgeo.faces[f].geometry.n(GRID_NOR, 2) =  gridgeo.faces[f].geometry.n(GRID_TAN1, 2);
                break;
                
            case TRI3:
                n3 = &gridgeo.Node(gridgeo.faces[f].connectivity.nodeIDs[2]);
                
                gridgeo.faces[f].geometry.S = area_triangle(n1->geometry.X, n2->geometry.X, n3->geometry.X);
                
                // Normal vector
                x1 = vectorStartEnd(n2->geometry.X, n1->geometry.X);
                x1.normalize();
                
                x2 = vectorStartEnd(n3->geometry.X, n1->geometry.X);
                x2.normalize();
                
                x3 = cross(x1, x2);
                for (int d = 0; d < 3; d++) gridgeo.faces[f].geometry.n(GRID_NOR, d) = x3(d);
                
                // Tangential Vector
                for (int d = 0; d < 3; d++) gridgeo.faces[f].geometry.n(GRID_TAN1, d) = x1(d);
                
                x2 = cross(x3, x1);
                for (int d = 0; d < 3; d++) gridgeo.faces[f].geometry.n(GRID_TAN2, d) = x2(d);
                break;
                
                
            case QUAD4:
                n3 = &gridgeo.Node(gridgeo.faces[f].connectivity.nodeIDs[2]);
                n4 = &gridgeo.Node(gridgeo.faces[f].connectivity.nodeIDs[3]);
                
                gridgeo.faces[f].geometry.S = area_quadrilateral(n1->geometry.X, n2->geometry.X, n3->geometry.X, n4->geometry.X);
                
                
                // Normal vector
                x1 = vectorStartEnd(n2->geometry.X, n1->geometry.X);
                x2 = vectorStartEnd(n3->geometry.X, n1->geometry.X);
                x3 = vectorStartEnd(n4->geometry.X, n1->geometry.X);
                
                x1.normalize();
                x2.normalize();
                x3.normalize();
                
                matrix x4 = normalFromThreePoints(x1, x2, x3);
                x4.normalize();
                
                
                for (int d = 0; d < 3; d++) gridgeo.faces[f].geometry.n(GRID_NOR, d) = x4(d);
                
                // Tangential Vector
                for (int d = 0; d < 3; d++) gridgeo.faces[f].geometry.n(GRID_TAN1, d) = x2(d);
                
                x1 = cross(x4, x2);
                for (int d = 0; d < 3; d++) gridgeo.faces[f].geometry.n(GRID_TAN2, d) = x1(d);
                break;
        }
        
        // Calculate X_center of the face
        for (int d = 0; d < gridinfo.DIM; d++)
        {
            gridgeo.faces[f].geometry.X(d) = 0.0;
            for (int n = 0; n < gridgeo.faces[f].connectivity.numNodes; n++)	gridgeo.faces[f].geometry.X(d) += gridgeo.Node(gridgeo.faces[f].connectivity.nodeIDs[n]).geometry.X(d);
            gridgeo.faces[f].geometry.X(d) /= gridgeo.faces[f].connectivity.numNodes;
        }
        
        //griddata.faces[f].area_inv = 1.0 / grid.faces[f].area;
    }
    
    
    // 3. Processing for Cell
#pragma ivdep
    for (int c = 0; c < gridinfo.NCM; c++)
    {
        CellBase *cell;
        NodeBase *node;
        cell = &gridgeo.cells[c];
        
        // Find Xc
        for (int d = 0; d < gridinfo.DIM; d++) cell->geometry.X(d) = 0.0;
        
        for (int n = 0; n < cell->connectivity.numNodes; n++)
        {
            node = &gridgeo.Node(cell->connectivity.nodeIDs[n]);
            
            for (int d = 0; d < gridinfo.DIM; d++) cell->geometry.X(d) += node->geometry.X(d);
            
        }
        
        for (int d = 0; d < gridinfo.DIM; d++) cell->geometry.X(d) /= cell->connectivity.numNodes;
        
        
        // Calculate Volume/Area and Characteristic length
        matrix xf1(3);
        matrix xf2(3);
        matrix xf3(3);
        matrix xf4(3);
        
        NodeBase* n1 = &gridgeo.Node(cell->connectivity.nodeIDs[0]);
        NodeBase* n2 = &gridgeo.Node(cell->connectivity.nodeIDs[1]);
        NodeBase* n3 = &gridgeo.Node(cell->connectivity.nodeIDs[2]);
        NodeBase* n4 = NULL;
        NodeBase* n5 = NULL;
        NodeBase* n6 = NULL;
        NodeBase* n7 = NULL;
        NodeBase* n8 = NULL;
        
        double aux1, aux2;
        
        switch (cell->connectivity.type)
        {
            case TRI3:
                cell->geometry.S = area_triangle(n1->geometry.X, n2->geometry.X, n3->geometry.X);
                cell->geometry.characteristicLength = sqrt(cell->geometry.S);
                break;
                
            case QUAD4:
                n4 = &gridgeo.Node(cell->connectivity.nodeIDs[3]);
                cell->geometry.S = area_quadrilateral(n1->geometry.X, n2->geometry.X, n3->geometry.X, n4->geometry.X);
                
                aux1 = area_line(n2->geometry.X, n1->geometry.X);
                aux2 = area_line(n4->geometry.X, n1->geometry.X);
                cell->geometry.characteristicLength = fmin(aux1, aux2);
                break;
                
            case TETRA4:
                n4 = &gridgeo.Node(cell->connectivity.nodeIDs[3]);
                cell->geometry.S = area_tetrahedron(n1->geometry.X, n2->geometry.X, n3->geometry.X, n4->geometry.X);
                
                for (int k = 0; k < gridinfo.DIM; k++)
                {
                    xf1(k)	= (n1->geometry.X(k) + n2->geometry.X(k) + n3->geometry.X(k)) / 3.0;
                    xf2(k)	= (n1->geometry.X(k) + n2->geometry.X(k) + n4->geometry.X(k)) / 3.0;
                    xf3(k)	= (n2->geometry.X(k) + n3->geometry.X(k) + n4->geometry.X(k)) / 3.0;
                    xf4(k)	= (n1->geometry.X(k) + n4->geometry.X(k) + n3->geometry.X(k)) / 3.0;
                    
                }
                
                aux1 = area_line(cell->geometry.X, xf1);
                aux1 = fmin(area_line(cell->geometry.X, xf2), aux1);
                aux1 = fmin(area_line(cell->geometry.X, xf3), aux1);
                aux1 = fmin(area_line(cell->geometry.X, xf4), aux1);
                cell->geometry.characteristicLength = 2.0 *  aux1;
                break;
                
            case HEXA8:
                n4 = &gridgeo.Node(cell->connectivity.nodeIDs[3]);
                n5 = &gridgeo.Node(cell->connectivity.nodeIDs[4]);
                n6 = &gridgeo.Node(cell->connectivity.nodeIDs[5]);
                n7 = &gridgeo.Node(cell->connectivity.nodeIDs[6]);
                n8 = &gridgeo.Node(cell->connectivity.nodeIDs[7]);
                
                cell->geometry.S = area_hexahedron(n1->geometry.X,
                                                   n2->geometry.X,
                                                   n3->geometry.X,
                                                   n4->geometry.X,
                                                   n5->geometry.X,
                                                   n6->geometry.X,
                                                   n7->geometry.X,
                                                   n8->geometry.X);
                
                aux1 = area_line(n2->geometry.X, n1->geometry.X);
                aux2 = area_line(n3->geometry.X, n1->geometry.X);
                aux1 = fmin(aux1, aux2);
                
                aux2 = area_line(n4->geometry.X, n1->geometry.X);
                cell->geometry.characteristicLength = fmin(aux1, aux2);
                break;
                
            case PRISM6:
                n4 = &gridgeo.Node(cell->connectivity.nodeIDs[3]);
                n5 = &gridgeo.Node(cell->connectivity.nodeIDs[4]);
                n6 = &gridgeo.Node(cell->connectivity.nodeIDs[5]);
                
                cell->geometry.S = area_prism(n1->geometry.X,
                                              n2->geometry.X,
                                              n3->geometry.X,
                                              n4->geometry.X,
                                              n5->geometry.X,
                                              n6->geometry.X);
                
                
                
                for (int k = 0; k < gridinfo.DIM; k++)
                {
                    xf1(k)	= n2->geometry.X(k) - n1->geometry.X(k);
                    xf2(k)	= n3->geometry.X(k) - n1->geometry.X(k);
                    xf3(k)	= n4->geometry.X(k) - n1->geometry.X(k);
                }
                
                aux1 =  pow(0.5*(xf1(1)*xf2(2) - xf1(2)*xf2(1)), 2.0);
                aux1 += pow(-0.5*(xf1(0)*xf2(2) - xf1(2)*xf2(0)), 2.0);
                aux1 += pow(0.5*(xf1(0)*xf2(1) - xf1(1)*xf2(0)), 2.0);
                aux1 = pow(aux1, 0.25);
                
                aux2 = area_line(n4->geometry.X, n1->geometry.X);
                cell->geometry.characteristicLength = fmin(aux1, aux2);
                break;
                
            case PYRAMID5:
                n4 = &gridgeo.Node(cell->connectivity.nodeIDs[3]);
                n5 = &gridgeo.Node(cell->connectivity.nodeIDs[4]);
                
                cell->geometry.S = area_pyramid(n1->geometry.X,
                                                n2->geometry.X,
                                                n3->geometry.X,
                                                n4->geometry.X,
                                                n5->geometry.X);
                
                cell->geometry.characteristicLength = pow(cell->geometry.S, 1.0/6.0);
                break;
        }
    }
    
    
    // 4. Create Ghost cells
    int counterNGM = 0;
    for (int f = 0; f < gridinfo.NFM; f++)
    {
        if (gridgeo.faces[f].connectivity.cr[0] == 0 || gridgeo.faces[f].connectivity.cl[0] == 0) counterNGM++;
    }
    
    gridgeo.ghost.resize(counterNGM);
    gridgeo.whereisGhost.resize(counterNGM+1);
    
    int gID = 1;
    int gPos = 0;
    
    FaceBase* face;
    CellBase* ghost;
    for (int f = 0; f < gridinfo.NFM; f++)
    {
        face = &gridgeo.faces[f];
        
        if (face->connectivity.cr[0] == 0)
        {
            face->connectivity.cr[0] = -gID;
            
            ghost = &gridgeo.ghost[gPos];
            gridgeo.whereisGhost[gID] = gPos;
            
            
            // Generate ghost cell
            ghost->geometry.id = -gID;
            ghost->geometry.bcType  = face->geometry.bcType;
            ghost->geometry.bcIndex = face->geometry.bcIndex;
            ghost->geometry.S       = face->geometry.S;
            
            for (int k = 0; k < gridinfo.DIM; k++)
            {
                ghost->geometry.X(k) = 2.0*face->geometry.X(k) - gridgeo.Cell(face->connectivity.cl[0]).geometry.X(k);
            }
            
            ghost->connectivity.type     = -1;
            ghost->connectivity.numNodes = face->connectivity.numNodes;
            ghost->connectivity.numFaces = 1;
            
            ghost->connectivity.nodeIDs.resize(ghost->connectivity.numNodes);
            ghost->connectivity.faceIDs.resize(ghost->connectivity.numFaces);
            for  (int n = 0; n < ghost->connectivity.numNodes; n++) ghost->connectivity.nodeIDs[n] = face->connectivity.nodeIDs[n];
            ghost->connectivity.faceIDs[0] = face->geometry.id;
            
            gPos++;
            gID++;
            if (gID+1 > gridgeo.whereisGhost.size()) gridgeo.whereisGhost.resize(gID+1);
        }
        else if (face->connectivity.cl[0] == 0)
        {
            face->connectivity.cl[0] = face->connectivity.cr[0];
            
            for (int k1 = 0; k1 < 3; k1++)
            {
                for (int k2 = 0; k2 < 3; k2++)
                {
                    face->geometry.n(k1,k2)	= -face->geometry.n(k1, k2);
                }
            }
            face->connectivity.cr[0] = -gID;
            
            ghost = &gridgeo.ghost[gPos];
            gridgeo.whereisGhost[gID] = gPos;
            
            
            // Generate ghost cell
            ghost->geometry.id      = -gID;
            ghost->geometry.bcType  = face->geometry.bcType;
            ghost->geometry.bcIndex = face->geometry.bcIndex;
            ghost->geometry.S       = face->geometry.S;
            
            for (int k = 0; k < gridinfo.DIM; k++)
            {
                ghost->geometry.X(k) = 2.0*face->geometry.X(k) - gridgeo.Cell(face->connectivity.cl[0]).geometry.X(k);
            }
            
            ghost->connectivity.type     = -1;
            ghost->connectivity.numNodes = face->connectivity.numNodes;
            ghost->connectivity.numFaces = 1;
            
            ghost->connectivity.nodeIDs.resize(ghost->connectivity.numNodes);
            ghost->connectivity.faceIDs.resize(ghost->connectivity.numFaces);
            for  (int n = 0; n < ghost->connectivity.numNodes; n++) ghost->connectivity.nodeIDs[n] = face->connectivity.nodeIDs[n];
            ghost->connectivity.faceIDs[0] = face->geometry.id;
            
            gPos++;
            gID++;
            if (gID+1 > gridgeo.whereisGhost.size()) gridgeo.whereisGhost.resize(gID+1);
        }
    }
    gridinfo.NGM = counterNGM;
    
    
    
    
    // 5. Calculate distance to wall
    // a Initizlize variables
    std::vector<unsigned int>       walkBound(gridgeo.whereisCells.size(), 0);			    // Walk bound of cell ID
    std::vector<FaceBase*>          nearWallFace(gridgeo.whereisCells.size(), NULL);	    // Near wall face of cell ID
    for (int c = 0; c < gridinfo.NCM; c++) gridgeo.cells[c].geometry.distanceToWall	= 1.E10;
    
    // b Calculate distance of 1st Wall layer
    int	num_WallFaces	= 0;
    int	num_WalkCells	= 0;
    
    for (int g = 0; g < gridinfo.NGM; g++)
    {
        if (isWallTypeBC(gridgeo.ghost[g].geometry.bcType))    // Wall BCs
        {
            num_WallFaces++;
            
            int fID = gridgeo.ghost[g].connectivity.faceIDs[0];
            int cID = gridgeo.Face(fID).connectivity.cl[0] + gridgeo.Face(fID).connectivity.cr[0] - gridgeo.ghost[g].geometry.id;
            
            walkBound[cID]    = 1;
            nearWallFace[cID] = &gridgeo.Face(fID);
            num_WalkCells++;
            
            gridgeo.Cell(cID).geometry.distanceToWall = area_line(gridgeo.Cell(cID).geometry.X, gridgeo.Face(fID).geometry.X);
            int temp;
        }
    }

    // c. Calculate path
    int num_Pass = 0;
    FaceBase* wall_Face;
    
    while (num_WalkCells > 0 && num_Pass <= GRID_MAX_NUM_PATH)
    {
        num_Pass++;
        num_WalkCells = 0;	// Initialize number of walk cells
        
        for (int c = 0; c < gridinfo.NCM; c++)
        {
            int cID   = gridgeo.cells[c].geometry.id;
            
            if (walkBound[cID] == num_Pass)
            {
                wall_Face = nearWallFace[cID];

                for (int f = 0; f < gridgeo.Cell(cID).connectivity.numFaces; f++)
                {
                    int fID = gridgeo.Cell(cID).connectivity.faceIDs[f];
                    int cl  = gridgeo.Face(fID).connectivity.cl[0] + gridgeo.Face(fID).connectivity.cr[0] - cID;
                    
                    
                    // Only for inner cell(Not ghost cells)
                    if (cl > 0)
                    {
                        if (walkBound[cl] != 1)	// Not for the first layer of walls
                        {
                            if (nearWallFace[cl] == NULL)	// Case: Near wall face is not allocated
                            {
                                nearWallFace[cl]	= wall_Face;
                                walkBound[cl]	    = num_Pass + 1;
                                num_WalkCells++;
                                
                                gridgeo.Cell(cl).geometry.distanceToWall = area_line(gridgeo.Cell(cl).geometry.X, wall_Face->geometry.X);
                            }
                            else if (nearWallFace[cl] != wall_Face)
                            {
                                double dist = area_line(gridgeo.Cell(cl).geometry.X, wall_Face->geometry.X);

                                if (dist < gridgeo.Cell(cl).geometry.distanceToWall)
                                {
                                    nearWallFace[cl] = wall_Face;
                                    walkBound[cl]	 = num_Pass + 1;
                                    num_WalkCells++;
                                    
                                    gridgeo.Cell(cl).geometry.distanceToWall   = dist;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // d. Treatment for ghost cells
    for (int g = 0; g < gridinfo.NGM; g++)
    {
        int fID = gridgeo.ghost[g].connectivity.faceIDs[0];
        int cl  = gridgeo.Face(fID).connectivity.cl[0] + gridgeo.Face(fID).connectivity.cr[0] - gridgeo.ghost[g].geometry.id;
        
        if (isWallTypeBC(gridgeo.ghost[g].geometry.bcType))
        {
            gridgeo.ghost[g].geometry.distanceToWall = -gridgeo.Cell(cl).geometry.distanceToWall;
        }
        else
        {
            gridgeo.ghost[g].geometry.distanceToWall = gridgeo.Cell(cl).geometry.distanceToWall;
        }
    }
    
    
    // 6. Calculate distance and n_dot_wallVector for each faces
    /*
	for (int f = 0; f < gridinfo.NFM; f++)
    {
        int fID = gridgeo.faces[f].geometry.id;
        int cl = gridgeo.Face(fID).connectivity.cl[0];
        int cr = gridgeo.Face(fID).connectivity.cr[0];
        
        gridgeo.Face(fID).geometry.distanceToWall	= 0.5 * (gridgeo.Cell(cl).geometry.distanceToWall + gridgeo.Cell(cr).geometry.distanceToWall);
        wall_Face = nearWallFace[cl];
        
        matrix wallNormal(3);
        matrix faceNormal(3);
		
        for (int d = 0; d < 3; d++)
        {
            wallNormal(d) = wall_Face->geometry.n(GRID_NOR, d);
            faceNormal(d) = gridgeo.Face(fID).geometry.n(GRID_NOR, d);
        }
        
        double n_dot_wall = dot(faceNormal, wallNormal);
        gridgeo.Face(fID).geometry.nDotWall = 1.0 - fabs(n_dot_wall);
    }
    */
    
    // 7. Find Stencil
    // Find stencil
    matrix xfc(3);
    matrix normal_vector(3);
    double max_dot;
    double test_dot;
    
    std::vector<std::vector <int> > neighborList;
    neighborList.resize(gridinfo.NCM + 1);
    
    for (int c = 0; c < gridinfo.NCM; c++)
    {
        int cID = gridgeo.cells[c].geometry.id;
        neighborList[cID].resize(gridgeo.cells[c].connectivity.numFaces);
        
        
        for (int f = 0; f < gridgeo.cells[c].connectivity.numFaces; f++)
        {
            int fID = gridgeo.cells[c].connectivity.faceIDs[f];
            int cl  = gridgeo.Face(fID).connectivity.cl[0];
            int cr  = gridgeo.Face(fID).connectivity.cr[0];
            
            if (cl == cID) neighborList[cID][f] = cr;
            else           neighborList[cID][f] = cl;
        }
    }
    
    
    for (int f = 0; f < gridinfo.NFM; f++)
    {
        int fID = gridgeo.faces[f].geometry.id;
        int cl  = gridgeo.Face(fID).connectivity.cl[0];
        int cr  = gridgeo.Face(fID).connectivity.cr[0];
        
        int cll;
        int crr;
        
        // 1: FIND CLL
        if (cl > 0)
        {
            cll     = 0;
            max_dot = -1.0;
            
            // GET NORMAL VECTOR TOWARD TO CL
            for (int k = 0; k < 3; k++) normal_vector(k) = -gridgeo.faces[f].geometry.n(GRID_NOR, k);
            
            
            // CALCULATE DISTANCE TO CELL-CENTER OF EACH NEIGHBORING CELL
            for (int j = 0; j < neighborList[cl].size(); j++)
            {
                int trial_ID = neighborList[cl][j];
                
                for (int k = 0; k < 3; k++) xfc(k) = gridgeo.Cell(trial_ID).geometry.X(k) - gridgeo.faces[f].geometry.X(k);
                xfc.normalize();
                
                test_dot = dot(normal_vector, xfc);
                if (test_dot > max_dot)
                {
                    max_dot = test_dot;
                    cll     = trial_ID;
                }
            }
            
            if (cll == 0) cll	= cl;
        }
        else
        {
            cll = cl;
        }
        
        
        
        // 2: FIND CRR
        if (cr > 0)
        {
            crr     = 0;
            max_dot = -1.0;
            
            // GET NORMAL VECTOR TOWARD TO CR
            for (int k = 0; k < 3; k++) normal_vector(k) = gridgeo.faces[f].geometry.n(GRID_NOR, k);
            
            // CALCULATE DISTANCE TO CELL-CENTER OF EACH NEIGHBORING CELL
            for (int j = 0; j < neighborList[cl].size(); j++)
            {
                int trial_ID = neighborList[cr][j];
                
                for (int k = 0; k < 3; k++) xfc(k) = gridgeo.Cell(trial_ID).geometry.X(k) - gridgeo.faces[f].geometry.X(k);
                xfc.normalize();
                
                test_dot = dot(normal_vector, xfc);
                if (test_dot > max_dot)
                {
                    max_dot = test_dot;
                    crr     = trial_ID;
                }
            }
            
            if (crr == 0) crr	= cr;
        }
        else
        {
            crr = cr;
        }
        
        gridgeo.faces[f].connectivity.cl.resize(2);
        gridgeo.faces[f].connectivity.cr.resize(2);
        
        gridgeo.faces[f].connectivity.cl[1] = cll;
        gridgeo.faces[f].connectivity.cr[1] = crr;
    }

}


// 3. Write Data
// 3.1 Tecplot Format
void writeGridGeoTecplot(const  std::string& title, GridBasicInfo& gridinfo, GridGeo& griddata)
{
    int maxNodeID = griddata.whereisNodes.size()-1;
    int maxCellID = griddata.whereisCells.size()-1;
    std::vector<int>    newNodeID(maxNodeID+1);
    std::vector<int>    newCellID(maxCellID+1);
    
    int NNM = 0;
    for (int n = 1; n <= maxNodeID; n++)
    {
        newNodeID[n] = -1;
        
        if (griddata.Node(n).geometry.treat != -1)
        {
            NNM++;
            newNodeID[n] = NNM;
        }
    }
    
    int NCM = 0;
    for (int c = 1; c <= maxCellID; c++)
    {
        if (griddata.Cell(c).geometry.treat != -1)
        {
            NCM++;
            newCellID[c] = NCM;
        }
    }
    
    // 1. Open file to write
    std::string fileName;
    fileName = title + ".plt";
    
    std::ofstream grid_tecplot;
    grid_tecplot.open(fileName.c_str());
    
    
    // 2. Write header
    grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
    grid_tecplot << "FILETYPE = GRID" << std::endl;
    if (gridinfo.DIM == 2)	     grid_tecplot << std::scientific << std::setprecision(16) << "VARIABLES = \"X\" \"Y\" " << std::endl;
    else if (gridinfo.DIM == 3)  grid_tecplot << std::scientific << std::setprecision(16) << "VARIABLES = \"X\" \"Y\" \"Z\" " << std::endl;
    
    grid_tecplot << "ZONE" << std::endl;
    grid_tecplot << "T=\"" << title <<"\"" << std::endl;
    grid_tecplot << "DATAPACKING= POINT" << std::endl;
    grid_tecplot << "NODES="    << NNM << std::endl;
    grid_tecplot << "ELEMENTS=" << NCM << std::endl;
    
    if (gridinfo.DIM == 2)	    grid_tecplot << "ZONETYPE = FEQUADRILATERAL" << std::endl;
    else if (gridinfo.DIM == 3) grid_tecplot << "ZONETYPE = FEBRICK" << std::endl;
    
    
    // 3. Write node Positions
    for (int n = 1; n <= maxNodeID; n++)
    {
        if (newNodeID[n] != -1)
        {
            for (int d = 0; d < gridinfo.DIM; d++)
            {
                grid_tecplot << griddata.Node(newNodeID[n]).geometry.X(d) << " ";
            }
            grid_tecplot << std::endl;
        }
    }
    grid_tecplot << std::endl;
    
    
    // 4. Write cell Data
    for (int c = 1; c <= maxCellID; c++)
    {
        if (newCellID[c] != -1)
        {
            int cID = newCellID[c];
            switch(griddata.Cell(cID).connectivity.type)
            {
                case TRI3:
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[0]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[1]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << std::endl;
                    break;
                    
                case QUAD4:
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[0]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[1]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[3]] << std::endl;
                    break;
                    
                case TETRA4:
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[0]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[1]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[3]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[3]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[3]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[3]] << std::endl;
                    break;
                    
                case HEXA8:
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[0]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[1]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[3]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[4]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[5]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[6]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[7]] << std::endl;
                    break;
                    
                case PRISM6:
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[0]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[1]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[3]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[4]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[5]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[5]] << std::endl;
                    break;
                    
                case PYRAMID5:
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[0]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[1]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[2]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[3]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[4]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[4]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[4]] << " ";
                    grid_tecplot << newNodeID[griddata.Cell(cID).connectivity.nodeIDs[4]] << std::endl;
                    break;
            }
        }
    }
    
    grid_tecplot.close();
}


void writeMeshTecplot(const std::string& title, Mesh& mesh)
{
	int maxNodeID = mesh.numNodes;
	int maxCellID = mesh.numCells;
	std::vector<int>    newNodeID(maxNodeID);
	std::vector<int>    newCellID(maxCellID);

	int NNM = 0;
	for (int n = 0; n < maxNodeID; n++)
	{
		newNodeID[n] = -1;

		if (mesh.nodesVector.nodes[n].geometry.treat != -1)
		{
			NNM++;
			newNodeID[n] = NNM;
		}
	}

	int NCM = 0;
	for (int c = 0; c < maxCellID; c++)
	{
		if (mesh.cellsVector.cells[c].geometry.treat != -1)
		{
			NCM++;
			newCellID[c] = NCM;
		}
	}

	// 1. Open file to write
	std::string fileName;
	fileName = title + ".plt";

	std::ofstream grid_tecplot;
	grid_tecplot.open(fileName.c_str());


	// 2. Write header
	grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
	grid_tecplot << "FILETYPE = GRID" << std::endl;
	if (mesh.dimension == 2)	     grid_tecplot << std::scientific << std::setprecision(16) << "VARIABLES = \"X\" \"Y\" " << std::endl;
	else if (mesh.dimension == 3)  grid_tecplot << std::scientific << std::setprecision(16) << "VARIABLES = \"X\" \"Y\" \"Z\" " << std::endl;

	grid_tecplot << "ZONE" << std::endl;
	grid_tecplot << "T=\"" << title << "\"" << std::endl;
	grid_tecplot << "DATAPACKING= POINT" << std::endl;
	grid_tecplot << "NODES=" << NNM << std::endl;
	grid_tecplot << "ELEMENTS=" << NCM << std::endl;

	if (mesh.dimension == 2)	    grid_tecplot << "ZONETYPE = FEQUADRILATERAL" << std::endl;
	else if (mesh.dimension == 3) grid_tecplot << "ZONETYPE = FEBRICK" << std::endl;


	// 3. Write node positions
	for (int n = 0; n < maxNodeID; n++)
	{
		if (newNodeID[n] != -1)
		{
			for (int d = 0; d < mesh.dimension; d++)
			{
				grid_tecplot << mesh.nodesVector.nodes[newNodeID[n] - 1].geometry.X(d) << " ";
			}
			grid_tecplot << std::endl;
		}
	}
	grid_tecplot << std::endl;


	// 4. Write cell data
	for (int c = 0; c < maxCellID; c++)
	{
		if (newCellID[c] != -1)
		{
			int cID = newCellID[c] - 1;

			grid_tecplot << newNodeID[mesh.cellsVector.cells[cID].connectivity.nodeIDs[0] - 1] << " ";
			grid_tecplot << newNodeID[mesh.cellsVector.cells[cID].connectivity.nodeIDs[1] - 1] << " ";
			grid_tecplot << newNodeID[mesh.cellsVector.cells[cID].connectivity.nodeIDs[2] - 1] << " ";
			grid_tecplot << newNodeID[mesh.cellsVector.cells[cID].connectivity.nodeIDs[3] - 1] << std::endl;
		}
	}

	grid_tecplot.close();
}


// Write Solution DATA
// Cell based data
void writeSolutionCellTecplot(const std::string& title, GridBasicInfo& gridinfo, GridGeo& griddata,
                              vector2D& data, std::vector<std::string>& variableNames, int N)
{
    // 1. Open file to write
    std::string fileName;
    fileName = title + ".plt";
    
    std::ofstream grid_tecplot;
    grid_tecplot.open(fileName.c_str());
    
    
    // 2. Write header
    grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
    grid_tecplot << "FILETYPE = SOLUTION" << std::endl;
    
    grid_tecplot << std::scientific << std::setprecision(16) << "VARIABLES = ";
    for (int i = 0; i < N; i++) grid_tecplot << " \" " << variableNames[i] << " \" ";
    grid_tecplot << std::scientific << std::endl;
    
    grid_tecplot << "ZONE T=\"" << title << " solution" << "\"" << std::endl;
    grid_tecplot << "DATAPACKING = BLOCK" << std::endl;
    grid_tecplot << "NODES = "    << gridinfo.NNM << std::endl;
    grid_tecplot << "ELEMENTS = " << gridinfo.NCM << std::endl;
    
    if (gridinfo.DIM == 2)	    grid_tecplot << "ZONETYPE = FEQUADRILATERAL" << std::endl;
    else if (gridinfo.DIM == 3) grid_tecplot << "ZONETYPE = FEBRICK" << std::endl;
    
    grid_tecplot << "VARLOCATION = ([1";
    for (int i = 1; i < N; i++) grid_tecplot << ", " << i+1;
    grid_tecplot << "] = CELLCENTERED)" << std::endl;

    
    // 3. Write solution data
    for (int i = 0; i < N; i++)
    {
        for (int c = 0; c < gridinfo.NCM; c++)
        {
            grid_tecplot << data[c][i] << std::endl;
        }
        
        grid_tecplot << std::endl;
    }
    
    grid_tecplot.close();
}


// Node based data
void writeSolutionNodeTecplot(const std::string& title, Mesh& mesh, double t)
{
	if (t == 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str());


		// 2. Write header
		grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
		grid_tecplot << "FILETYPE = FULL" << std::endl;
		grid_tecplot << "VARIABLES = \"X\" \"Y\" \"Rho\" \"Phi\" \"E\" \"B\" \"NodeID\" \"Time\"" << std::endl;
		grid_tecplot << "ZONE T = \"" << t << " seconds\", DATAPACKING = POINT, NODES = " <<
			mesh.numNodes << ", ELEMENTS = " << mesh.numCells << ", SOLUTIONTIME = " <<
			t << ", ZONETYPE = FEQUADRILATERAL, STRANDID = 1" << std::endl;


		// 3. Write solution data
		for (int c = 0; c < mesh.numNodes; c++)
		{
			grid_tecplot << std::scientific << std::setprecision(16) <<
				mesh.nodesVector.nodes[c].geometry.X(0) << " " <<
				mesh.nodesVector.nodes[c].geometry.X(1) << " " <<
				mesh.nodesVector.nodes[c].rho << " " <<
				mesh.nodesVector.nodes[c].phi << " " <<
				sqrt(mesh.nodesVector.nodes[c].EMfield[0] * mesh.nodesVector.nodes[c].EMfield[0] +
					mesh.nodesVector.nodes[c].EMfield[1] * mesh.nodesVector.nodes[c].EMfield[1] +
					mesh.nodesVector.nodes[c].EMfield[2] * mesh.nodesVector.nodes[c].EMfield[2]) << " " <<
				sqrt(mesh.nodesVector.nodes[c].EMfield[3] * mesh.nodesVector.nodes[c].EMfield[3] +
					mesh.nodesVector.nodes[c].EMfield[4] * mesh.nodesVector.nodes[c].EMfield[4] +
					mesh.nodesVector.nodes[c].EMfield[5] * mesh.nodesVector.nodes[c].EMfield[5]) << " " <<
				c + 1 << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;


		// 4. Write connectivity data
		for (int c = 0; c < mesh.numCells; c++)
		{
				grid_tecplot << mesh.cellsVector.cells[c].connectivity.nodeIDs[0] << " ";
				grid_tecplot << mesh.cellsVector.cells[c].connectivity.nodeIDs[1] << " ";
				grid_tecplot << mesh.cellsVector.cells[c].connectivity.nodeIDs[2] << " ";
				grid_tecplot << mesh.cellsVector.cells[c].connectivity.nodeIDs[3] << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
	else if (t > 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str(), std::ios::app);


		// 2. Write header
		grid_tecplot << "ZONE T = \"" << t << " seconds\", DATAPACKING = POINT, NODES = " <<
			mesh.numNodes << ", ELEMENTS = " << mesh.numCells << ", SOLUTIONTIME = " <<
			t << ", ZONETYPE = FEQUADRILATERAL, STRANDID = 1" << 
			", CONNECTIVITYSHAREZONE = 1, VARSHARELIST = ([1,2] = 1)" << std::endl;

		// 3. Write solution data
		for (int c = 0; c < mesh.numNodes; c++)
		{
			grid_tecplot << std::scientific << std::setprecision(16) <<
				mesh.nodesVector.nodes[c].rho << " " <<
				mesh.nodesVector.nodes[c].phi << " " <<
				sqrt(mesh.nodesVector.nodes[c].EMfield[0] * mesh.nodesVector.nodes[c].EMfield[0] +
					mesh.nodesVector.nodes[c].EMfield[1] * mesh.nodesVector.nodes[c].EMfield[1] +
					mesh.nodesVector.nodes[c].EMfield[2] * mesh.nodesVector.nodes[c].EMfield[2]) << " " <<
				sqrt(mesh.nodesVector.nodes[c].EMfield[3] * mesh.nodesVector.nodes[c].EMfield[3] +
					mesh.nodesVector.nodes[c].EMfield[4] * mesh.nodesVector.nodes[c].EMfield[4] +
					mesh.nodesVector.nodes[c].EMfield[5] * mesh.nodesVector.nodes[c].EMfield[5]) << " " <<
				c + 1 << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}

}


// Point based data (plot all N points at each time step t)
void writeSolutionXY_T_Tecplot(const std::string& title, vector2D& data, int N, double t)
{
	if (t == 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str());


		// 2. Write header
		grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
		grid_tecplot << "FILETYPE = FULL" << std::endl;
		grid_tecplot << "VARIABLES = \"X\" \"Y\" \"U\" \"V\" \"CellID\" \"ParticleID\" \"Type\" \"Time\"" << std::endl;
		grid_tecplot << "ZONE DATAPACKING = POINT, T =\"" << t << " seconds\", I =" << N << std::endl;


		// 3. Write solution data

		for (int i = 0; i < N; i++)
		{
			grid_tecplot << std::scientific << std::setprecision(16) << data[i][0]
				<< " " << data[i][1] << " " << data[i][2] << " " << data[i][3]
				<< " " << static_cast<int>(data[i][4]) << " " << static_cast<int>(data[i][5])
				<< " " << static_cast<int>(data[i][6]) << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
	else if (t > 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str(), std::ios::app);


		// 2. Write header
		grid_tecplot << "ZONE DATAPACKING = POINT, T = \"" << t << " seconds\", I =" << N << std::endl;


		// 3. Write solution data

		for (int i = 0; i < N; i++)
		{
			grid_tecplot << std::scientific << std::setprecision(16) << data[i][0]
				<< " " << data[i][1] << " " << data[i][2] << " " << data[i][3]
				<< " " << static_cast<int>(data[i][4]) << " " << static_cast<int>(data[i][5])
				<< " " << static_cast<int>(data[i][6]) << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
}


// Point based data (plot all N points at each time step t, animated)
void writeSolutionXY_TA_Tecplot(const std::string& title, vector2D& data, int N, double t)
{
	if (t == 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str());


		// 2. Write header
		grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
		grid_tecplot << "FILETYPE = FULL" << std::endl;
		grid_tecplot << "VARIABLES = \"X\" \"Y\" \"U\" \"V\" \"CellID\" \"ParticleID\" \"Type\" \"Time\"" << std::endl;
		grid_tecplot << "ZONE DATAPACKING = POINT, T =\"" << t << " seconds\", I =" << N << ", SOLUTIONTIME = " << t << std::endl;


		// 3. Write solution data

		for (int i = 0; i < N; i++)
		{
			grid_tecplot << std::scientific << std::setprecision(16) << data[i][0]
				<< " " << data[i][1] << " " << data[i][2] << " " << data[i][3]
				<< " " << static_cast<int>(data[i][4]) << " " << static_cast<int>(data[i][5])
				<< " " << static_cast<int>(data[i][6]) << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
	else if (t > 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str(), std::ios::app);


		// 2. Write header
		grid_tecplot << "ZONE DATAPACKING = POINT, T = \"" << t << " seconds\", I =" << N << ", SOLUTIONTIME = " << t << std::endl;


		// 3. Write solution data

		for (int i = 0; i < N; i++)
		{
			grid_tecplot << std::scientific << std::setprecision(16) << data[i][0]
				<< " " << data[i][1] << " " << data[i][2] << " " << data[i][3]
				<< " " << static_cast<int>(data[i][4]) << " " << static_cast<int>(data[i][5])
				<< " " << static_cast<int>(data[i][6]) << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
}


// Point based data (follow N individual points for each time step t)
void writeSolutionXY_NT_Tecplot(const std::string& title, vector2D& data, int N, double t)
{
	if (t == 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str());


		// 2. Write header
		grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
		grid_tecplot << "FILETYPE = FULL" << std::endl;
		grid_tecplot << "VARIABLES = \"X\" \"Y\" \"U\" \"V\" \"CellID\" \"ParticleID\" \"Type\" \"Time\"" << std::endl;
		

		// 3. Write solution data

		for (int i = 0; i < N; i++)
		{
			grid_tecplot << "ZONE DATAPACKING = POINT, T = \"Particle " << static_cast<int>(data[i][5]) << "\", I = 1, STRANDID = " << static_cast<int>(data[i][5]) << std::endl;
			grid_tecplot << std::scientific << std::setprecision(16) << data[i][0]
				<< " " << data[i][1] << " " << data[i][2] << " " << data[i][3] 
				<< " " << static_cast<int>(data[i][4]) << " " << static_cast<int>(data[i][5]) 
				<< " " << static_cast<int>(data[i][6]) << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
	else if (t > 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str(), std::ios::app);


		// 2. Write solution data

		for (int i = 0; i < N; i++)
		{
			grid_tecplot << "ZONE DATAPACKING = POINT, T = \"Particle " << static_cast<int>(data[i][5]) << "\", I = 1, STRANDID = " << static_cast<int>(data[i][5]) << std::endl;
			grid_tecplot << std::scientific << std::setprecision(16) << data[i][0]
				<< " " << data[i][1] << " " << data[i][2] << " " << data[i][3]
				<< " " << static_cast<int>(data[i][4]) << " " << static_cast<int>(data[i][5])
				<< " " << static_cast<int>(data[i][6]) << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
}


// Point based data (follow N individual points for each time step t, animated)
void writeSolutionXY_NTA_Tecplot(const std::string& title, vector2D& data, int N, double t)
{
	if (t == 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str());


		// 2. Write header
		grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
		grid_tecplot << "FILETYPE = FULL" << std::endl;
		grid_tecplot << "VARIABLES = \"X\" \"Y\" \"U\" \"V\" \"CellID\" \"ParticleID\" \"Type\" \"Time\"" << std::endl;


		// 3. Write solution data

		for (int i = 0; i < N; i++)
		{
			grid_tecplot << "ZONE DATAPACKING = POINT, T = \"Particle " << static_cast<int>(data[i][5]) << "\", I = 1, STRANDID = " << static_cast<int>(data[i][5]) << ", SOLUTIONTIME = " << t << std::endl;
			grid_tecplot << std::scientific << std::setprecision(16) << data[i][0]
				<< " " << data[i][1] << " " << data[i][2] << " " << data[i][3]
				<< " " << static_cast<int>(data[i][4]) << " " << static_cast<int>(data[i][5])
				<< " " << static_cast<int>(data[i][6]) << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
	else if (t > 0.0)
	{
		// 1. Open File to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str(), std::ios::app);


		// 2. Write solution data

		for (int i = 0; i < N; i++)
		{
			grid_tecplot << "ZONE DATAPACKING = POINT, T = \"Particle " << static_cast<int>(data[i][5]) << "\", I = 1, STRANDID = " << static_cast<int>(data[i][5]) << ", SOLUTIONTIME = " << t << std::endl;
			grid_tecplot << std::scientific << std::setprecision(16) << data[i][0]
				<< " " << data[i][1] << " " << data[i][2] << " " << data[i][3]
				<< " " << static_cast<int>(data[i][4]) << " " << static_cast<int>(data[i][5])
				<< " " << static_cast<int>(data[i][6]) << " " << t << std::scientific << std::endl;
		}
		grid_tecplot << std::endl;

		grid_tecplot.close();
	}
}

// Time based data (plot data from over the course of the simulation)
void writeSolution_T_Tecplot(const std::string& title, double EK, double EP, int N, double t)
{
	if (t == 0.0)
	{
		// 1. Open file to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str());


		// 2. Write header
		grid_tecplot << "TITLE = \"" << title << "\"" << std::endl;
		grid_tecplot << "FILETYPE = FULL" << std::endl;
		grid_tecplot << "VARIABLES = \"EK\" \"EP\" \"Total\" \"Time\"" << std::endl;


		// 3. Write solution data

		grid_tecplot << "ZONE DATAPACKING = POINT, I = " << N + 1 << std::endl;
		grid_tecplot << std::scientific << std::setprecision(16) << EK << " " <<
			EP << " " << EK + abs(EP) << " " << t << std::scientific <<
			std::endl;

		grid_tecplot.close();
	}
	else if (t > 0.0)
	{
		// 1. Open File to write
		std::string fileName;
		fileName = title + ".plt";

		std::ofstream grid_tecplot;
		grid_tecplot.open(fileName.c_str(), std::ios::app);


		// 2. Write solution data

		grid_tecplot << std::scientific << std::setprecision(16) << EK << " " <<
			EP << " " << EK + abs(EP) << " " << t << std::scientific <<
			std::endl;

		grid_tecplot.close();
	}
}