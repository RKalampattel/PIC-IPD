//
//  grid.hpp
//  OP2A
//
//  Created by Kim M.K. on 09/11/2016.
//  Last edited by Rahul Kalampattel, April 2018
//  
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef grid_hpp
#define grid_hpp

#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <string>
#include <stdio.h>

#include "../COMM/assert_mk.hpp"
#include "../COMM/readdatafromstring.hpp"
#include "../COMM/error_codes.hpp"
#include "../COMM/error_exception.hpp"
#include "../COMM/StringOps.hpp"
#include "../COMM/VectorCompare.hpp"

#include "../MATH_MK/matrix.hpp"
#include "../MATH_MK/cal_area.hpp"
#include "../MATH_MK/linear_algebra.hpp"

#include "element.hpp"
#include "node.hpp"
#include "face.hpp"
#include "cell.hpp"
#include "grid_def.hpp"

#include "../Mesh.h"

///////////////////////
// Classes for Grid
///////////////////////
// 1. Class for BASIC INFO
class GridBasicInfo{
    // Part A: Data Section
public:
    double gridFactor;
    int isAxisymmeric;
    int DIM;
    int NNM;
    int NFM;
    int NCM;
    int NGM;
    
protected:
    bool m_completed;
   
    // Part B: Constructoir / Destructor Section
public:
    GridBasicInfo();
    ~GridBasicInfo();
    
    // Part C: Functions
public:
    bool isCompleted();
    void setComplete();
    void needToUpdate();
    void read(const std::string& mesh_file_name);
};


// 2. Class for MPI Data
class GridMPI
{
    // Part A: Data Section
public:
    int numLines;                                       // Number of lines
    std::vector< std::vector<int> > lines;
    std::vector< std::vector<int> > sendGhostIndex;
    std::vector< std::vector<int> > receiveGhostIndex;
    int *sendGhost;
    int *receiveGhost;
    std::vector<int> cellProcessorIDs;                  // Processor IDs of each cells
    
    // Part B: Constructoir / Destructor Section
public:
    GridMPI();
    ~GridMPI();
    
};



// 3. Base class for Grid-class (Ancestor)
class GridGeo{
    // [Part A]: Data section
public:
    
    // DATA for Grid Geom
    std::vector<NodeBase>   nodes;
    std::vector<FaceBase>   faces;
    std::vector<CellBase>   cells;
    std::vector<CellBase>   ghost;
    
    std::vector<int>        whereisNodes;
    std::vector<int>        whereisFaces;
    std::vector<int>        whereisCells;
    std::vector<int>        whereisGhost;
    
    // [Part B]: Constructor / Destructor section
public:
    GridGeo();
    GridGeo(GridBasicInfo& gridInfo);
    ~GridGeo();
    
    // [Part C]: Functions
public:
    NodeBase& Node(int id);
    FaceBase& Face(int id);
    CellBase& Cell(int id);
    
    void readGrid(const std::string& mesh_file_name, GridBasicInfo& gridInfo);
};


// 5. Grid for Data
template <class DATA>
class GridData{
    // [Part A]: Data section
    
public:
    std::vector<DATA>   nodes;
    std::vector<DATA>   faces;
    std::vector<DATA>   cells;
    std::vector<DATA>   ghost;
    
    // [Part B]: Constructor / Destructor section
public:
    GridData() { };
    GridData(GridBasicInfo& gridInfo)
    : nodes(gridInfo.NNM), faces(gridInfo.NFM), cells(gridInfo.NCM), ghost(gridInfo.NGM)
    {
        
    };
    
    GridData(GridBasicInfo* gridInfo)
    : nodes(gridInfo->NNM), faces(gridInfo->NFM), cells(gridInfo->NCM), ghost(gridInfo->NGM)
    {
        
    };
    
    
    
    ~GridData() { };

public:
    // [Part C]: Functions
    void allocate(GridBasicInfo& gridInfo)
    {
        nodes.resize(gridInfo.NNM);
        faces.resize(gridInfo.NFM);
        cells.resize(gridInfo.NCM);
        ghost.resize(gridInfo.NGM);
    }
};

// 6. Grid for Data Vector
template <class DATA>
class GridDataVec{
    // [Part A]: Data section
    
public:
    std::vector< std::vector<DATA> >   nodes;
    std::vector< std::vector<DATA> >   faces;
    std::vector< std::vector<DATA> >   cells;
    std::vector< std::vector<DATA> >   ghost;
    
    // [Part B]: Constructor / Destructor section
public:
    GridDataVec() { };
    GridDataVec(GridBasicInfo& gridInfo, int n_fluid)
    : nodes(gridInfo.NNM, std::vector<DATA>(n_fluid)),
      faces(gridInfo.NFM, std::vector<DATA>(n_fluid)),
      cells(gridInfo.NCM, std::vector<DATA>(n_fluid)),
      ghost(gridInfo.NGM, std::vector<DATA>(n_fluid))
    {
        
        
    };
    
    GridDataVec(GridBasicInfo* gridInfo, int n_fluid)
    : nodes(gridInfo->NNM, std::vector<DATA>(n_fluid)),
      faces(gridInfo->NFM, std::vector<DATA>(n_fluid)),
      cells(gridInfo->NCM, std::vector<DATA>(n_fluid)),
      ghost(gridInfo->NGM, std::vector<DATA>(n_fluid))
    {
        
    };
    
    
    
    ~GridDataVec() { };
    
public:
    // [Part C]: Functions
    void allocate(GridBasicInfo& gridInfo, int n_fluid)
    {
        nodes.resize(gridInfo.NNM); for (int n = 0; n < gridInfo.NNM; n++) nodes[n].resize(n_fluid);
        faces.resize(gridInfo.NFM); for (int f = 0; f < gridInfo.NFM; f++) faces[f].resize(n_fluid);
        cells.resize(gridInfo.NCM); for (int c = 0; c < gridInfo.NCM; c++) cells[c].resize(n_fluid);
        ghost.resize(gridInfo.NGM); for (int g = 0; g < gridInfo.NFM; g++) ghost[g].resize(n_fluid);
    }
};


//////////////////////
// Grid Utilities
//////////////////////
int faceTypeNumNode(int facetype);
int cellTypeNumNode(int celltype);
int cellTypeNumFace(int celltype);
bool isWallTypeBC(int bc);

void faceNodeNumberingReverse(std::vector<int>& nodeList, int type);
int faceCompare(std::vector<int>& F1_list, std::vector<int>& F2_list, int type); // [1] Same [-1] Different direction [0] Different

///////////////////////////////////////////////////////////////////////////////
// Grid file read and processing
///////////////////////////////////////////////////////////////////////////////

// 1. Pre-processing
// 1.1 SU2 input files
void precessingGridSU2(const std::string& mesh_file_name_SU2);
void precessingGridSU2(const std::string& mesh_file_name_SU2, std::string& out_file_name);
// 1.2 FLUENT input files
void precessingGridFLUENT(const std::string& mesh_file_name_Fluent);
void precessingGridFLUENT(const std::string& mesh_file_name_Fluent, std::string& out_file_name);

///////////////////////////////////////////////////////////////////////////////

// 2. Processing
void readGridFromFile(const std::string& mesh_file_name, GridBasicInfo& gridinfo, GridGeo& gridgeo);
void processingGrid(GridBasicInfo& gridinfo, GridGeo& gridgeo);

///////////////////////////////////////////////////////////////////////////////

// 3. Post-processing
// 3.1 Mesh data
void writeGridGeoTecplot(const  std::string& title, GridBasicInfo& gridinfo, GridGeo& griddata);
void writeMeshTecplot(const std::string& title, Mesh& mesh);
// 3.2 Solution data
void writeSolutionCellTecplot(const std::string& title, GridBasicInfo& gridinfo, 
	GridGeo& griddata, vector2D& data, std::vector<std::string>& variableNames, int N);		// Cell based data
void writeSolutionNodeTecplot(const std::string& title, Mesh& mesh, double t);				// Node based data
void writeSolutionXY_T_Tecplot(const std::string& title, vector2D& data, int N, double t);	// Point based data (plot all N points at each time step t)
void writeSolutionXY_TA_Tecplot(const std::string& title, vector2D& data, int N, double t);	// Point based data (plot all N points at each time step t, animated)
void writeSolutionXY_NT_Tecplot(const std::string& title, vector2D& data, int N, double t); // Point based data (follow N individual points for each time step t)
void writeSolutionXY_NTA_Tecplot(const std::string& title, vector2D& data, int N, double t);// Point based data (follow N individual points for each time step t, animated)
void writeSolution_T_Tecplot(const std::string& title, double EK, double EP, int N, double t);// Time based data (plot data from over the course of the simulation)

///////////////////////////////////////////////////////////////////////////////

/*
// 3. class for Geometry and Datastrorage
template <class N, class F, class C>
class GridData
{
public:
    std::vector<N>   nodes;
    std::vector<F>   faces;
    std::vector<C>   cells;
    std::vector<C>   ghost;
    
    std::vector<int>    whereisNodes;
    std::vector<int>    whereisFaces;
    std::vector<int>    whereisCells;
    std::vector<int>    whereisGhost;
    
public:
    GridData()
    {
        
    };
    
    GridData(GridBasicInfo& gridInfo)
    : nodes(gridInfo.NNM), faces(gridInfo.NFM), cells(gridInfo.NCM)
    {
        
    };
    
    ~GridData()
    {
        
    };
    

    N& Node(int id);
    F& Face(int id);
    C& Cell(int id);
    void readGrid(const std::string& mesh_file_name, GridBasicInfo& gridInfo);
};
*/





/*
// ========================
// Definition of Grid-class
// ========================
// Base class for Grid-class (Ancestor)
class GridBase{
    // [Part A]: Data section
public:
    GridBasicInfo       info;
    GridMPI             mpi;
    
    // [Part B]: Constructor / Destructor section
public:
    virtual ~GridBase() {}
    
    // [Part C]: Functions
public:
    virtual void readGrid(const std::string& mesh_file_name) = 0;
    virtual void writeGridGeo(const std::string& title)      = 0;
};


template <class N, class F, class C>
class Grid : public GridBase
{
public:
    GridData<N, F, C>   data;
    
    Grid()  {};
    ~Grid() {};
    
    void readGrid(const std::string& mesh_file_name);
    void writeGridGeo(const std::string& title);
};


// Functions
//  4.1 readGrid
template <class N, class F, class C>
void Grid<N, F, C>::readGrid(const std::string& mesh_file_name)
{
    info.read(mesh_file_name);
    data.readGrid(mesh_file_name, info);
    processingGrid(info, data);
}

// 4.2 writeGridTecplot
template <class N, class F, class C>
void Grid<N, F, C>::writeGridGeo(const  std::string& title)
{
    writeGridGeoTecplot(title, info, data);
}










template <class N, class F, class C>
N& GridData<N, F, C>::Node(int id)
{
    op_assert(id > 0);
    int npos = whereisNodes[id];
    return (nodes[npos]);
}

template <class N, class F, class C>
F& GridData<N, F, C>::Face(int id)
{
    op_assert(id > 0);
    int npos = whereisFaces[id];
    return (faces[npos]);
}

template <class N, class F, class C>
C& GridData<N, F, C>::Cell(int id)
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




template <class N, class F, class C>
void GridData<N, F, C>::readGrid(const std::string& mesh_file_name, GridBasicInfo& gridInfo)
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




// Grid Functions
// 1. Read grid
template <class N, class F, class C>
void readGridFromFile(const std::string& mesh_file_name, GridBasicInfo& gridinfo, GridData<N, F, C>& griddata)
{
    griddata.readGrid(mesh_file_name, gridinfo);
}


// 2. Processing grid
template <class N, class F, class C>
void processingGrid(GridBasicInfo& gridinfo, GridData<N, F, C>& griddata)
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
                griddata.nodes[n].geometry.X(k) /= gridinfo.gridFactor;
            }
        }
        
    }
    
    if (gridinfo.isAxisymmeric == true && gridinfo.DIM == 2)
    {
#pragma ivdep
        for (int n = 0; n < gridinfo.NNM; n++)
        {
            griddata.nodes[n].geometry.X(1) = fabs(griddata.nodes[n].geometry.X(1)) + GRID_AXISMEMTRIC_Y;
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
        
        N* n1 = &griddata.Node(griddata.faces[f].connectivity.nodeIDs[0]);
        N* n2 = &griddata.Node(griddata.faces[f].connectivity.nodeIDs[1]);
        N* n3 = NULL;
        N* n4 = NULL;
        
        switch (griddata.faces[f].connectivity.type)
        {
            case LINE:
                griddata.faces[f].geometry.S = area_line(n1->geometry.X, n2->geometry.X);
                
                // Tangential vector
                x1 = vectorStartEnd(n1->geometry.X, n2->geometry.X);
                x1.normalize();
                for (int d = 0; d < 3; d++) griddata.faces[f].geometry.n(GRID_TAN1, d) = x1(d);
                
                // Normal Vector
                griddata.faces[f].geometry.n(GRID_NOR, 0) =  griddata.faces[f].geometry.n(GRID_TAN1, 1);
                griddata.faces[f].geometry.n(GRID_NOR, 1) = -griddata.faces[f].geometry.n(GRID_TAN1, 0);
                griddata.faces[f].geometry.n(GRID_NOR, 2) =  griddata.faces[f].geometry.n(GRID_TAN1, 2);
                break;
                
            case TRI3:
                n3 = &griddata.Node(griddata.faces[f].connectivity.nodeIDs[2]);
                
                griddata.faces[f].geometry.S = area_triangle(n1->geometry.X, n2->geometry.X, n3->geometry.X);
                
                // Normal vector
                x1 = vectorStartEnd(n2->geometry.X, n1->geometry.X);
                x1.normalize();
                
                x2 = vectorStartEnd(n3->geometry.X, n1->geometry.X);
                x2.normalize();
                
                x3 = cross(x1, x2);
                for (int d = 0; d < 3; d++) griddata.faces[f].geometry.n(GRID_NOR, d) = x3(d);
                
                // Tangential Vector
                for (int d = 0; d < 3; d++) griddata.faces[f].geometry.n(GRID_TAN1, d) = x1(d);
                
                x2 = cross(x3, x1);
                for (int d = 0; d < 3; d++) griddata.faces[f].geometry.n(GRID_TAN2, d) = x2(d);
                break;
                
                
            case QUAD4:
                n3 = &griddata.Node(griddata.faces[f].connectivity.nodeIDs[2]);
                n4 = &griddata.Node(griddata.faces[f].connectivity.nodeIDs[3]);
                
                griddata.faces[f].geometry.S = area_quadrilateral(n1->geometry.X, n2->geometry.X, n3->geometry.X, n4->geometry.X);
                
                
                // Normal vector
                x1 = vectorStartEnd(n2->geometry.X, n1->geometry.X);
                x2 = vectorStartEnd(n3->geometry.X, n1->geometry.X);
                x3 = vectorStartEnd(n4->geometry.X, n1->geometry.X);
                
                x1.normalize();
                x2.normalize();
                x3.normalize();
                
                matrix x4 = normalFromThreePoints(x1, x2, x3);
                x4.normalize();
                
                
                for (int d = 0; d < 3; d++) griddata.faces[f].geometry.n(GRID_NOR, d) = x4(d);
                
                // Tangential Vector
                for (int d = 0; d < 3; d++) griddata.faces[f].geometry.n(GRID_TAN1, d) = x2(d);
                
                x1 = cross(x4, x2);
                for (int d = 0; d < 3; d++) griddata.faces[f].geometry.n(GRID_TAN2, d) = x1(d);
                break;
        }
        
        // Calculate X_center of the face
        for (int d = 0; d < gridinfo.DIM; d++)
        {
            griddata.faces[f].geometry.X(d) = 0.0;
            for (int n = 0; n < griddata.faces[f].connectivity.numNodes; n++)	griddata.faces[f].geometry.X(d) += griddata.Node(griddata.faces[f].connectivity.nodeIDs[n]).geometry.X(d);
            griddata.faces[f].geometry.X(d) /= griddata.faces[f].connectivity.numNodes;
        }
        
        //griddata.faces[f].area_inv = 1.0 / grid.faces[f].area;
    }
    
    
    // 3. Processing for Cell
#pragma ivdep
    for (int c = 0; c < gridinfo.NCM; c++)
    {
        C *cell;
        N *node;
        cell = &griddata.cells[c];
        
        // Find Xc
        for (int d = 0; d < gridinfo.DIM; d++) cell->geometry.X(d) = 0.0;
        
        for (int n = 0; n < cell->connectivity.numNodes; n++)
        {
            node = &griddata.Node(cell->connectivity.nodeIDs[n]);
            
            for (int d = 0; d < gridinfo.DIM; d++) cell->geometry.X(d) += node->geometry.X(d);
            
        }
        
        for (int d = 0; d < gridinfo.DIM; d++) cell->geometry.X(d) /= cell->connectivity.numNodes;
        
        
        // Calculate Volume/Area and Characteristic length
        matrix xf1(3);
        matrix xf2(3);
        matrix xf3(3);
        matrix xf4(3);
        
        N* n1 = &griddata.Node(cell->connectivity.nodeIDs[0]);
        N* n2 = &griddata.Node(cell->connectivity.nodeIDs[1]);
        N* n3 = &griddata.Node(cell->connectivity.nodeIDs[2]);
        N* n4 = NULL;
        N* n5 = NULL;
        N* n6 = NULL;
        N* n7 = NULL;
        N* n8 = NULL;
        
        double aux1, aux2;
        
        switch (cell->connectivity.type)
        {
            case TRI3:
                cell->geometry.S = area_triangle(n1->geometry.X, n2->geometry.X, n3->geometry.X);
                cell->geometry.characteristicLength = sqrt(cell->geometry.S);
                break;
                
            case QUAD4:
                n4 = &griddata.Node(cell->connectivity.nodeIDs[3]);
                cell->geometry.S = area_quadrilateral(n1->geometry.X, n2->geometry.X, n3->geometry.X, n4->geometry.X);
                
                aux1 = area_line(n2->geometry.X, n1->geometry.X);
                aux2 = area_line(n4->geometry.X, n1->geometry.X);
                cell->geometry.characteristicLength = fmin(aux1, aux2);
                break;
                
            case TETRA4:
                n4 = &griddata.Node(cell->connectivity.nodeIDs[3]);
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
                n4 = &griddata.Node(cell->connectivity.nodeIDs[3]);
                n5 = &griddata.Node(cell->connectivity.nodeIDs[4]);
                n6 = &griddata.Node(cell->connectivity.nodeIDs[5]);
                n7 = &griddata.Node(cell->connectivity.nodeIDs[6]);
                n8 = &griddata.Node(cell->connectivity.nodeIDs[7]);
                
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
                n4 = &griddata.Node(cell->connectivity.nodeIDs[3]);
                n5 = &griddata.Node(cell->connectivity.nodeIDs[4]);
                n6 = &griddata.Node(cell->connectivity.nodeIDs[5]);
                
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
                n4 = &griddata.Node(cell->connectivity.nodeIDs[3]);
                n5 = &griddata.Node(cell->connectivity.nodeIDs[4]);
                
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
        if (griddata.faces[f].connectivity.cr[0] == 0 || griddata.faces[f].connectivity.cl[0] == 0) counterNGM++;
    }
    
    griddata.ghost.resize(counterNGM);
    griddata.whereisGhost.resize(counterNGM+1);
    
    int gID = 1;
    int gPos = 0;
    
    F* face;
    C* ghost;
    for (int f = 0; f < gridinfo.NFM; f++)
    {
        face = &griddata.faces[f];
        
        if (face->connectivity.cr[0] == 0)
        {
            face->connectivity.cr[0] = -gID;
            
            ghost = &griddata.ghost[gPos];
            griddata.whereisGhost[gID] = gPos;
            
            
            // Generate ghost cell
            ghost->geometry.id = -gID;
            ghost->geometry.bcType  = face->geometry.bcType;
            ghost->geometry.bcIndex = face->geometry.bcIndex;
            ghost->geometry.S       = face->geometry.S;
            
            for (int k = 0; k < gridinfo.DIM; k++)
            {
                ghost->geometry.X(k) = 2.0*face->geometry.X(k) - griddata.Cell(face->connectivity.cl[0]).geometry.X(k);
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
            if (gID+1 > griddata.whereisGhost.size()) griddata.whereisGhost.resize(gID+1);
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
            
            ghost = &griddata.ghost[gPos];
            griddata.whereisGhost[gID] = gPos;
            
            
            // Generate ghost cell
            ghost->geometry.id      = -gID;
            ghost->geometry.bcType  = face->geometry.bcType;
            ghost->geometry.bcIndex = face->geometry.bcIndex;
            ghost->geometry.S       = face->geometry.S;
            
            for (int k = 0; k < gridinfo.DIM; k++)
            {
                ghost->geometry.X(k) = 2.0*face->geometry.X(k) - griddata.Cell(face->connectivity.cl[0]).geometry.X(k);
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
            if (gID+1 > griddata.whereisGhost.size()) griddata.whereisGhost.resize(gID+1);
        }
    }
    gridinfo.NGM = counterNGM;
    
    
    
    
    // 5. Calculate distance to wall
    // a Initizlize variables
    std::vector<unsigned int>              walkBound(griddata.whereisCells.size(), 0);			    // Walk bound of cell ID
    std::vector< F* >     nearWallFace(griddata.whereisCells.size(), NULL);	    // Near wall face of cell ID
    for (int c = 0; c < gridinfo.NCM; c++) griddata.cells[c].geometry.distanceToWall	= 1.E10;
    
    // b Calculate distance of 1st Wall layer
    int	num_WallFaces	= 0;
    int	num_WalkCells	= 0;
    
    for (int g = 0; g < gridinfo.NGM; g++)
    {
        if (isWallTypeBC(griddata.ghost[g].geometry.bcType))    // Wall BCs
        {
            num_WallFaces++;
            
            int fID = griddata.ghost[g].connectivity.faceIDs[0];
            int cID = griddata.Face(fID).connectivity.cl[0] + griddata.Face(fID).connectivity.cr[0] - griddata.ghost[g].geometry.id;
            
            walkBound[cID]    = 1;
            nearWallFace[cID] = &griddata.Face(fID);
            num_WalkCells++;
            
            griddata.Cell(cID).geometry.distanceToWall = area_line(griddata.Cell(cID).geometry.X, griddata.Face(fID).geometry.X);
            int temp;
        }
    }
    
    // c. Calculate path
    int num_Pass = 0;
    F* wall_Face;
    
    while (num_WalkCells > 0 && num_Pass <= GRID_MAX_NUM_PATH)
    {
        num_Pass++;
        num_WalkCells = 0;	// Initialize number of walk cells
        
        for (int c = 0; c < gridinfo.NCM; c++)
        {
            int cID   = griddata.cells[c].geometry.id;
            
            if (walkBound[cID] == num_Pass)
            {
                wall_Face = nearWallFace[cID];
                
                for (int f = 0; f < griddata.Cell(cID).connectivity.numFaces; f++)
                {
                    int fID = griddata.Cell(cID).connectivity.faceIDs[f];
                    int cl  = griddata.Face(fID).connectivity.cl[0] + griddata.Face(fID).connectivity.cr[0] - cID;
                    
                    
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
                                
                                griddata.Cell(cl).geometry.distanceToWall = area_line(griddata.Cell(cl).geometry.X, wall_Face->geometry.X);
                            }
                            else if (nearWallFace[cl] != wall_Face)
                            {
                                double dist = area_line(griddata.Cell(cl).geometry.X, wall_Face->geometry.X);
                                
                                if (dist < griddata.Cell(cl).geometry.distanceToWall)
                                {
                                    nearWallFace[cl] = wall_Face;
                                    walkBound[cl]	 = num_Pass + 1;
                                    num_WalkCells++;
                                    
                                    griddata.Cell(cl).geometry.distanceToWall   = dist;
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
        int fID = griddata.ghost[g].connectivity.faceIDs[0];
        int cl  = griddata.Face(fID).connectivity.cl[0] + griddata.Face(fID).connectivity.cr[0] - griddata.ghost[g].geometry.id;
        
        if (isWallTypeBC(griddata.ghost[g].geometry.bcType))
        {
            griddata.ghost[g].geometry.distanceToWall = -griddata.Cell(cl).geometry.distanceToWall;
        }
        else
        {
            griddata.ghost[g].geometry.distanceToWall = griddata.Cell(cl).geometry.distanceToWall;
        }
    }
    
    
    // 6. Calculate distance and n_dot_wallVector for each faces
    for (int f = 0; f < gridinfo.NFM; f++)
    {
        int fID = griddata.faces[f].geometry.id;
        int cl = griddata.Face(fID).connectivity.cl[0];
        int cr = griddata.Face(fID).connectivity.cr[0];
        
        griddata.Face(fID).geometry.distanceToWall	= 0.5 * (griddata.Cell(cl).geometry.distanceToWall + griddata.Cell(cr).geometry.distanceToWall);
        wall_Face = nearWallFace[cl];
        
        matrix wallNormal(3);
        matrix faceNormal(3);
        
        for (int d = 0; d < 3; d++)
        {
            wallNormal(d) = wall_Face->geometry.n(GRID_NOR, d);
            faceNormal(d) = griddata.Face(fID).geometry.n(GRID_NOR, d);
        }
        
        double n_dot_wall = dot(faceNormal, wallNormal);
        griddata.Face(fID).geometry.nDotWall = 1.0 - fabs(n_dot_wall);
    }
    
    
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
        int cID = griddata.cells[c].geometry.id;
        neighborList[cID].resize(griddata.cells[c].connectivity.numFaces);
        
        
        for (int f = 0; f < griddata.cells[c].connectivity.numFaces; f++)
        {
            int fID = griddata.cells[c].connectivity.faceIDs[f];
            int cl  = griddata.Face(fID).connectivity.cl[0];
            int cr  = griddata.Face(fID).connectivity.cr[0];
            
            if (cl == cID) neighborList[cID][f] = cr;
            else           neighborList[cID][f] = cl;
        }
    }
    
    
    for (int f = 0; f < gridinfo.NFM; f++)
    {
        int fID = griddata.faces[f].geometry.id;
        int cl  = griddata.Face(fID).connectivity.cl[0];
        int cr  = griddata.Face(fID).connectivity.cr[0];
        
        int cll;
        int crr;
        
        // 1: FIND CLL
        if (cl > 0)
        {
            cll     = 0;
            max_dot = -1.0;
            
            // GET NORMAL VECTOR TOWARD TO CL
            for (int k = 0; k < 3; k++) normal_vector(k) = -griddata.faces[f].geometry.n(GRID_NOR, k);
            
            
            // CALCULATE DISTANCE TO CELL-CENTER OF EACH NEIGHBORING CELL
            for (int j = 0; j < neighborList[cl].size(); j++)
            {
                int trial_ID = neighborList[cl][j];
                
                for (int k = 0; k < 3; k++) xfc(k) = griddata.Cell(trial_ID).geometry.X(k) - griddata.faces[f].geometry.X(k);
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
            for (int k = 0; k < 3; k++) normal_vector(k) = griddata.faces[f].geometry.n(GRID_NOR, k);
            
            // CALCULATE DISTANCE TO CELL-CENTER OF EACH NEIGHBORING CELL
            for (int j = 0; j < neighborList[cl].size(); j++)
            {
                int trial_ID = neighborList[cr][j];
                
                for (int k = 0; k < 3; k++) xfc(k) = griddata.Cell(trial_ID).geometry.X(k) - griddata.faces[f].geometry.X(k);
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
        
        griddata.faces[f].connectivity.cl.resize(2);
        griddata.faces[f].connectivity.cr.resize(2);
        
        griddata.faces[f].connectivity.cl[1] = cll;
        griddata.faces[f].connectivity.cr[1] = crr;
    }
}


// 3. Write Data
// 3.1 Tecplot Format
template <class N, class F, class C>
void writeGridGeoTecplot(const  std::string& title, GridBasicInfo& gridinfo, GridData<N, F, C >& griddata)
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
    
    // 1. Open File to write
    std::string fileName;
    fileName = title + ".plt";
    
    std::ofstream grid_tecplot;
    grid_tecplot.open(fileName.c_str());
    
    
    // 2. Write Header
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
    
    
    // 3. Write Node Positions
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
    
    
    // 4. Write Cell Data
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

*/




#endif /* grid_hpp */
