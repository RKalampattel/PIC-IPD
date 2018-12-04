//
//  preprocessingGridSU2.cpp
//  OP2A
//
//  Created by Kim M.K. on 09/11/2016.
//  Last edited by Rahul Kalampattel, April 2018
//
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <iomanip>


#include "grid.hpp"
#include "../COMM/readdatafromstring.hpp"
#include "../COMM/error_codes.hpp"
#include "../COMM/error_exception.hpp"
#include "../COMM/StringOps.hpp"


void precessingGridSU2(const  std::string& mesh_file_name_SU2)
{
    int temp_dummy;
    int readIntTemp;
    int numFace;
    int numNode;
    int numBCZone;
    std::string line;
    
    
    
    
    // 1. Open file to read
    std::ifstream mesh_file_SU2;
    mesh_file_SU2.open(mesh_file_name_SU2.c_str());
    
    
    // 1st READ
    
    int dim;
    int nnm;
    int nfm;
    int ncm;
    int nbc;
    
    
    if (mesh_file_SU2.is_open())
    {
        while (! mesh_file_SU2.eof())
        {
            getline(mesh_file_SU2, line);
            
            readIntTemp = -1;
            readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NDIME=");
            if (readIntTemp != -1)	dim	=readIntTemp;
            
            // CELL
            readIntTemp = -1;
            readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NELEM=");
            if (readIntTemp != -1) ncm = readIntTemp;
            
            // NODE
            readIntTemp = -1;
            readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NPOIN=");
            if (readIntTemp != -1) nnm = readIntTemp;
            
            // BCs
            readIntTemp = -1;
            readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NMARK=");
            if (readIntTemp != -1) nbc = readIntTemp;
        }
    }
    else
    {
        Common::ExceptionError(FromHere(), "Cannot find grid file. Please check your grid file!", Common::ErrorCodes::FileSystem());
    }
    
    
    // 2. Set variables and 2nd read
    std::vector<int>               cell_type(ncm, -1);
    std::vector<int>               cell_ID(ncm, -1);
    std::vector<std::vector<int> > cell_NodeList(ncm, std::vector<int>(MAX_CELL_PER_NODE, -1));
    std::vector<std::vector<int> > cell_FaceList(ncm, std::vector<int>(MAX_CELL_PER_FACE, -1));
    
    std::vector<int>	                node_ID(nnm,-1);
    std::vector<std::vector<double> >	node_XY(nnm, std::vector<double>(dim, 0.0));
    
    std::vector <int> BCZone_type(nbc, -1);
    std::vector <int> BCZone_nele(nbc, -1);
    
    mesh_file_SU2.clear();
    mesh_file_SU2.seekg(0, std::ios::beg);
    
    
    while (! mesh_file_SU2.eof())
    {
        getline(mesh_file_SU2, line);
        
        // CELL
        readIntTemp = -1;
        readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NELEM=");
        if (readIntTemp != -1)
        {
            int temp_cellType;
            int temp_cellNode[8];
            int temp_cellID;
            
            for (int c=0; c <= ncm-1; c++)
            {
                
                getline(mesh_file_SU2, line);
                sscanf_s(line.c_str(), "%d", &temp_cellType);
                
                switch(temp_cellType)
                {
                    case TRI3:
                        sscanf_s(line.c_str(), "%d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                        
                    case QUAD4:
                        sscanf_s(line.c_str(), "%d %d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellNode[3],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_NodeList[temp_cellID][3] = temp_cellNode[3];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                        
                    case TETRA4:
                        sscanf_s(line.c_str(), "%d %d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellNode[3],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_NodeList[temp_cellID][3] = temp_cellNode[3];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                        
                    case HEXA8:
                        sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellNode[3],
                               &temp_cellNode[4],
                               &temp_cellNode[5],
                               &temp_cellNode[6],
                               &temp_cellNode[7],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_NodeList[temp_cellID][3] = temp_cellNode[3];
                        cell_NodeList[temp_cellID][4] = temp_cellNode[4];
                        cell_NodeList[temp_cellID][5] = temp_cellNode[5];
                        cell_NodeList[temp_cellID][6] = temp_cellNode[6];
                        cell_NodeList[temp_cellID][7] = temp_cellNode[7];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                        
                    case PRISM6:
                        sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellNode[3],
                               &temp_cellNode[4],
                               &temp_cellNode[5],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_NodeList[temp_cellID][3] = temp_cellNode[3];
                        cell_NodeList[temp_cellID][4] = temp_cellNode[4];
                        cell_NodeList[temp_cellID][5] = temp_cellNode[5];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                }
            }
        }
        
        
        // NODE
        readIntTemp = -1;
        readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NPOIN=");
        if (readIntTemp != -1)
        {
            int temp_nodeID;
            double tempX[3];
            
            if (dim == 2)
            {
                for (int n=0; n <= nnm-1; n++)
                {
                    getline(mesh_file_SU2, line);
                    sscanf_s(line.c_str(), "%lf %lf %d", &tempX[0], &tempX[1], &temp_nodeID);
                    
                    node_XY[temp_nodeID][0] = tempX[0];
                    node_XY[temp_nodeID][1] = tempX[1];
                    node_ID[temp_nodeID]	= temp_nodeID;
                }
            }
            else if (dim == 3)
            {
                for (int n=0; n <= nnm-1; n++)
                {
                    getline(mesh_file_SU2, line);
                    sscanf_s(line.c_str(), "%lf %lf %lf %d", &tempX[0], &tempX[1], &tempX[2], &temp_nodeID);
                    
                    node_XY[temp_nodeID][0] = tempX[0];
                    node_XY[temp_nodeID][1] = tempX[1];
                    node_XY[temp_nodeID][2] = tempX[2];
                    node_ID[temp_nodeID]	= temp_nodeID;
                }
            }
        }
        
        // BCs
        readIntTemp = -1;
        readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NMARK=");
        if (readIntTemp != -1)
        {
            for (int bc = 0; bc <= nbc-1; bc++)
            {
                getline(mesh_file_SU2, line);
                BCZone_type[bc] = Common::read_data_from_string::read_numeric<int>(line, "MARKER_TAG= bc-");
                
                getline(mesh_file_SU2, line);
                BCZone_nele[bc] = Common::read_data_from_string::read_numeric<int>(line, "MARKER_ELEMS=");
                
                for (int f = 0; f <= BCZone_nele[bc]-1; f++)
                {
                    getline(mesh_file_SU2, line);
                }
            }
        }
    }
    
    
    
    // 4. Estimate max nfm
    nfm = 0;
    for (int c = 0; c <= ncm-1; c++)
    {
        switch (cell_type[c])
        {
            case TRI3:
                nfm += 3;
                break;
                
            case QUAD4:
                nfm += 4;
                break;
                
            case TETRA4:
                nfm += 4;
                break;
                
            case HEXA8:
                nfm += 6;
                break;
                
            case PRISM6:
                nfm += 5;
                break;
        }
    }
    
    
    // 5. Construct faces
    std::vector<int>               face_type (nfm, -1);
    std::vector<int>               face_ID(nfm, -1);
    std::vector<int>               face_ID_real(nfm, -1);
    std::vector<int>               face_BC(nfm, 0);
    std::vector<bool>              face_include(nfm, true);
    std::vector<std::vector<int> > face_stencil(nfm, std::vector<int>(2, -1));
    std::vector<std::vector<int> > face_node(nfm, std::vector<int>(MAX_FACE_PER_NODE, -1));
    
    nfm = 0;
    for (int c = 0; c <= ncm-1; c++)
    {
        switch (cell_type[c])
        {
            case TRI3:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][2];
                face_node[nfm][1]    = cell_NodeList[c][0];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][2]	 = nfm;
                nfm++;
                break;
                
            case QUAD4:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][2];
                face_node[nfm][1]    = cell_NodeList[c][3];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][2]	 = nfm;
                nfm++;
                
                // 4rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][3];
                face_node[nfm][1]    = cell_NodeList[c][0];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][3]	 = nfm;
                nfm++;
                break;
                
            case TETRA4:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_node[nfm][2]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][3];
                face_node[nfm][2]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][3];
                face_node[nfm][2]    = cell_NodeList[c][4];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][2]	 = nfm;
                nfm++;
                
                // 4th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][2];
                face_node[nfm][2]    = cell_NodeList[c][3];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][3]	 = nfm;
                nfm++;
                break;
                
            case HEXA8:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_node[nfm][2]    = cell_NodeList[c][2];
                face_node[nfm][3]    = cell_NodeList[c][3];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][4];
                face_node[nfm][1]    = cell_NodeList[c][7];
                face_node[nfm][2]    = cell_NodeList[c][6];
                face_node[nfm][3]    = cell_NodeList[c][5];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][4];
                face_node[nfm][2]    = cell_NodeList[c][5];
                face_node[nfm][3]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][3]	 = nfm;
                nfm++;
                
                // 4th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][5];
                face_node[nfm][2]    = cell_NodeList[c][6];
                face_node[nfm][3]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][4]	 = nfm;
                nfm++;
                
                // 5th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][2];
                face_node[nfm][1]    = cell_NodeList[c][6];
                face_node[nfm][2]    = cell_NodeList[c][7];
                face_node[nfm][3]    = cell_NodeList[c][3];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][4]	 = nfm;
                nfm++;
                
                // 6th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][3];
                face_node[nfm][1]    = cell_NodeList[c][7];
                face_node[nfm][2]    = cell_NodeList[c][4];
                face_node[nfm][3]    = cell_NodeList[c][0];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][5]	 = nfm;
                nfm++;
                break;
                
            case PRISM6:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_node[nfm][2]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][3];
                face_node[nfm][1]    = cell_NodeList[c][5];
                face_node[nfm][2]    = cell_NodeList[c][4];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][3];
                face_node[nfm][2]    = cell_NodeList[c][4];
                face_node[nfm][3]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][2]	 = nfm;
                nfm++;
                
                // 4th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][4];
                face_node[nfm][2]    = cell_NodeList[c][5];
                face_node[nfm][3]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][3]	 = nfm;
                nfm++;
                
                // 5th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][2];
                face_node[nfm][1]    = cell_NodeList[c][5];
                face_node[nfm][2]    = cell_NodeList[c][3];
                face_node[nfm][3]    = cell_NodeList[c][0];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][4]	 = nfm;
                nfm++;
        }
    }
    
    
    // 6. Remove duplicated faces
    for (int f = 0; f <= nfm-1; f++)
    {
        if (face_include[f] == true)
        {
            for (int f1 = 0; f1 <= nfm-1; f1++)
            {
                if (f1 != f && face_include[f1] == true && face_type[f] == face_type[f1])
                {
                    switch (faceCompare(face_node[f], face_node[f1], face_type[f]))
                    {
                        case 1:
                            face_include[f1] = false;
                            face_ID[f1]  	 = f;
                            break;
                            
                        case -1:
                            face_include[f1] = false;
                            face_ID[f1]      = f;
                            face_stencil[f][1] = face_stencil[f1][0];
                            break;
                    }
                }
            }
        }
    }
    
    
    // 3. Set BC variables and 3rd read
    mesh_file_SU2.clear();
    mesh_file_SU2.seekg(0, std::ios::beg);
    
    while (! mesh_file_SU2.eof())
    {
        getline(mesh_file_SU2, line);
        // BCs
        readIntTemp = -1;
        readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NMARK=");
        if (readIntTemp != -1)
        {
            for (int bc = 0; bc <= nbc-1; bc++)
            {
                getline(mesh_file_SU2, line);
                BCZone_type[bc] = Common::read_data_from_string::read_numeric<int>(line, "MARKER_TAG= bc-");
                
                getline(mesh_file_SU2, line);
                BCZone_nele[bc] = Common::read_data_from_string::read_numeric<int>(line, "MARKER_ELEMS=");
                
                for (int f = 0; f <= BCZone_nele[bc]-1; f++)
                {
                    int temp_BC_EleType;
                    std::vector<int> temp_BC_EleNodeList(MAX_FACE_PER_NODE);
                    
                    getline(mesh_file_SU2, line);
                    sscanf_s(line.c_str(), "%d", &temp_BC_EleType);
                    
                    switch(temp_BC_EleType)
                    {
                        case LINE:
                            sscanf_s(line.c_str(), "%d %d %d", &temp_dummy, &temp_BC_EleNodeList[0],  &temp_BC_EleNodeList[1]);
                            break;
                            
                        case TRI3:
                            sscanf_s(line.c_str(), "%d %d %d %d", &temp_dummy,
                                   &temp_BC_EleNodeList[0],
                                   &temp_BC_EleNodeList[1],
                                   &temp_BC_EleNodeList[2]);
                            break;
                            
                        case QUAD4:
                            sscanf_s(line.c_str(), "%d %d %d %d %d", &temp_dummy,
                                   &temp_BC_EleNodeList[0],
                                   &temp_BC_EleNodeList[1],
                                   &temp_BC_EleNodeList[2],
                                   &temp_BC_EleNodeList[3]);
                            break;
                    }
                    
                    
                    for (int f1 = 0; f1 <= nfm-1; f1++)
                    {
                        if (face_include[f1] == true && face_type[f1] == temp_dummy)
                        {
                            if (faceCompare(face_node[f1], temp_BC_EleNodeList, temp_dummy) != 0)
                            {
                                face_BC[f1] = BCZone_type[bc];
                            }
                        }
                    }
                }
            }
        }
    }
    
    
    mesh_file_SU2.close();
    
    
    // Update real Face IDs
    int nfm_real = 0;
    int temp_ID;
    
    for (int f = 0; f <= nfm-1; f++)
    {
        if (face_BC[f] == 0)
        {
            if (face_include[f] == true)
            {
                temp_ID = face_ID[f];
                face_ID_real[temp_ID] = nfm_real;
                nfm_real++;
            }
        }
    }
    
    
    for (int f = 0; f <= nfm-1; f++)
    {
        if (face_BC[f] != 0)
        {
            if (face_include[f] == true)
            {
                temp_ID = face_ID[f];
                face_ID_real[temp_ID] = nfm_real;
                nfm_real++;
            }
        }
    }
    
    
    for (int f = 0; f <= nfm-1; f++)
    {
        temp_ID = face_ID[f];
        if (face_ID_real[f] == -1)	face_ID_real[f] = face_ID_real[temp_ID];
    }
    
    
    std::vector<int> face_RealID_to_Pos (nfm_real, -1);
    for (int f = 0; f <= nfm-1; f++)
    {
        temp_ID = face_ID_real[f];
        if (face_RealID_to_Pos[temp_ID] == -1) face_RealID_to_Pos[temp_ID] = f;
    }
    
    
    for (int c = 0; c <= ncm-1; c++)
    {
        int nface;
        switch (cell_type[c])
        {
            case TRI3:
                nface = 3;
                break;
                
            case QUAD4:
                nface = 4;
                break;
                
            case TETRA4:
                nface = 4;
                break;
                
            case HEXA8:
                nface = 6;
                break;
                
            case PRISM6:
                nface = 5;
                break;
                
        }
        
        for (int f = 0; f <= nface-1; f++)
        {
            int tempID = face_ID[cell_FaceList[c][f]];
            cell_FaceList[c][f] = face_ID_real[tempID];
        }
    }
    
    
    // WRITE OP2A GRID DATA
    std::string fileName;
    std::vector<std::string> fileName_temp;
    
    Common::StringOps::split(mesh_file_name_SU2, '.', fileName_temp);
    fileName = fileName_temp[0] + ".op2";
    
    std::ofstream mesh_file_OP2;
    mesh_file_OP2.open(fileName.c_str());
    
    
    
    
    mesh_file_OP2 << "%*****************************************************" << std::endl;
    mesh_file_OP2 << "%     OP2A Grid File using FORMAT version 1.0         " << std::endl;
    mesh_file_OP2 << "%                                                     " << std::endl;
    mesh_file_OP2 << "%                  Originally developed by Minkwan Kim" << std::endl;
    mesh_file_OP2 << "%=====================================================" << std::endl;
    mesh_file_OP2 << "% [FACE / CELL TYPE identifiers]                      " << std::endl;
    mesh_file_OP2 << "%      Line                   :  3                    " << std::endl;
    mesh_file_OP2 << "%      Triangle (3-nodes)     :  5                    " << std::endl;
    mesh_file_OP2 << "%      Quadrilateral (4-nodes):  9                    " << std::endl;
    mesh_file_OP2 << "%      Tetrahedral (4-nodes)  : 10                    " << std::endl;
    mesh_file_OP2 << "%      Hexahedral (8-nodes)   : 12                    " << std::endl;
    mesh_file_OP2 << "%      Prism (6-nodes)        : 13                    " << std::endl;
    mesh_file_OP2 << "%      Pramid (5-nodes)       : 14                    " << std::endl;
    mesh_file_OP2 << "%                                                     " << std::endl;
    mesh_file_OP2 << "% [BC TYPE identifiers]                               " << std::endl;
    mesh_file_OP2 << "%      Interior               :  0                    " << std::endl;
    mesh_file_OP2 << "%      Wall                   : 10                    " << std::endl;
    mesh_file_OP2 << "%      Inlet                  : 20                    " << std::endl;
    mesh_file_OP2 << "%      Outlet                 : 30                    " << std::endl;
    mesh_file_OP2 << "%      Freestream (Far-field) : 40                    " << std::endl;
    mesh_file_OP2 << "%      Symmetric              : 50                    " << std::endl;
    mesh_file_OP2 << "%      Axisymmetric           : 60                    " << std::endl;
    mesh_file_OP2 << "%      Anode                  : 70                    " << std::endl;
    mesh_file_OP2 << "%      Cathode                : 80                    " << std::endl;
    mesh_file_OP2 << "%      Dielectric material    : 90                    " << std::endl;
    mesh_file_OP2 << "%                                                     " << std::endl;
    mesh_file_OP2 << "% [DATA Format]                                       " << std::endl;
    mesh_file_OP2 << "%    -Node Data FORMAT                                " << std::endl;
    mesh_file_OP2 << "%     (NODE ID#)  (POSITION DATA: X Y Z)           " << std::endl;
    mesh_file_OP2 << "%    -Face Data FORMAT                                " << std::endl;
    mesh_file_OP2 << "%     (FACE ID#)  (BOUNDARY CONDITION)  (FACE TYPE)  (LIST OF NODE IDs) (LEFT-CELL ID) (RIGHT-CELL ID)" << std::endl;
    mesh_file_OP2 << "%    -Cell Data FORMAT" << std::endl;
    mesh_file_OP2 << "%     (CELL ID#)  (BOUNDARY CONDITION)  (CELL TYPE)  (LIST OF NODE IDs) (LIST OF FACE IDs)" << std::endl;
    mesh_file_OP2 << "%=====================================================" << std::endl;
    mesh_file_OP2 << "%                   Last Format Updated on April/2016 " << std::endl;
    mesh_file_OP2 << "%                                       by M Kim      " << std::endl;
    mesh_file_OP2 << "%*****************************************************" << std::endl << std::endl;
    
    mesh_file_OP2 << "% OVERALL GRID INFORMATION" << std::endl;
    mesh_file_OP2 << "DIM= " << dim << std::endl;
    mesh_file_OP2 << "NNM= " << nnm << std::endl;
    mesh_file_OP2 << "NFM= " << nfm_real << std::endl;
    mesh_file_OP2 << "NCM= " << ncm << std::endl;
    mesh_file_OP2 << std::endl;
    
    mesh_file_OP2 << "%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "NODEDATA= " << nnm << std::endl;
    if (dim == 2)
    {
        double z = 0.0;
        for (int n = 0; n <= nnm-1; n++)
            mesh_file_OP2 << std::setw(8) << node_ID[n]+1 <<
            std::setw(25) << std::scientific << std::setprecision(16) << node_XY[n][0] <<
            std::setw(25) << node_XY[n][1] <<
            std::setw(25) << z << std::endl;
    }
    else
    {
        for (int n = 0; n <= nnm-1; n++)
            mesh_file_OP2 << std::setw(8) << node_ID[n]+1<<
            std::setw(25) << std::scientific << std::setprecision(16) << node_XY[n][0] <<
            std::setw(25) << node_XY[n][1] <<
            std::setw(25) << node_XY[n][2] << std::endl;
    }
    mesh_file_OP2 << std::endl;
    
    mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "FACEDATA= " << nfm << std::endl;
    for (int f1 = 0; f1 <= nfm_real-1; f1++)
    {
        int f = face_RealID_to_Pos[f1];
        
        int nnode;
        switch (face_type[f])
        {
            case LINE:
                nnode = 2;
                break;
                
            case TRI3:
                nnode = 3;
                break;
                
            case QUAD4:
                nnode = 4;
                break;
        }
        
        mesh_file_OP2 << std::setw(8) << face_ID_real[f]+1;
        mesh_file_OP2 << std::setw(8) << face_BC[f];
        mesh_file_OP2 << std::setw(8) << face_type[f];
        
        for (int n = 0; n <= nnode-1; n++) mesh_file_OP2 << std::setw(8) << face_node[f][n] + 1;
        mesh_file_OP2 << std::setw(8) << face_stencil[f][0] + 1;
        mesh_file_OP2 << std::setw(8) << face_stencil[f][1] + 1;
        mesh_file_OP2 << std::endl;
    }
    mesh_file_OP2 << std::endl;
    
    
    
    mesh_file_OP2 << "%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "CELLDATA= " << ncm << std::endl;
    for (int c = 0; c <= ncm-1; c++)
    {
        int cellBC = 0;	// Todo: Need to improve later
        
        int nface;
        int nnode;
        
        switch (cell_type[c])
        {
            case TRI3:
                nface = 3;
                nnode = 3;
                break;
                
            case QUAD4:
                nface = 4;
                nnode = 4;
                break;
                
            case TETRA4:
                nface = 4;
                nnode = 4;
                break;
                
            case HEXA8:
                nface = 6;
                nnode = 8;
                break;
                
            case PRISM6:
                nface = 5;
                nnode = 6;
                break;
        }
        
        mesh_file_OP2 << std::setw(8) << cell_ID[c]+1;
        mesh_file_OP2 << std::setw(8) << cellBC;
        mesh_file_OP2 << std::setw(8) << cell_type[c];
        for (int n = 0; n <= nnode-1; n++) mesh_file_OP2 << std::setw(8) << cell_NodeList[c][n] + 1;
        for (int f = 0; f <= nface-1; f++) mesh_file_OP2 << std::setw(8) << cell_FaceList[c][f] + 1;
        mesh_file_OP2 << std::endl;
    }
    mesh_file_OP2 << std::endl;
    mesh_file_OP2.close();
}



void precessingGridSU2(const std::string& mesh_file_name_SU2, std::string& out_file_name)
{
    int temp_dummy;
    int readIntTemp;
    int numFace;
    int numNode;
    int numBCZone;
    std::string line;
    
    
    
    
    // 1. Open file to read
    std::ifstream mesh_file_SU2;
    mesh_file_SU2.open(mesh_file_name_SU2.c_str());
    
    
    // 1st READ
    
    int dim;
    int nnm;
    int nfm;
    int ncm;
    int nbc;
    
    
    if (mesh_file_SU2.is_open())
    {
        while (! mesh_file_SU2.eof())
        {
            getline(mesh_file_SU2, line);
            
            readIntTemp = -1;
            readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NDIME=");
            if (readIntTemp != -1)	dim	=readIntTemp;
            
            // CELL
            readIntTemp = -1;
            readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NELEM=");
            if (readIntTemp != -1) ncm = readIntTemp;
            
            // NODE
            readIntTemp = -1;
            readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NPOIN=");
            if (readIntTemp != -1) nnm = readIntTemp;
            
            // BCs
            readIntTemp = -1;
            readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NMARK=");
            if (readIntTemp != -1) nbc = readIntTemp;
        }
    }
    else
    {
        Common::ExceptionError(FromHere(), "Cannot find grid file. Please check your grid file!", Common::ErrorCodes::FileSystem());
    }
    
    
    // 2. Set variables and 2nd read
    std::vector<int>               cell_type(ncm, -1);
    std::vector<int>               cell_ID(ncm, -1);
    std::vector<std::vector<int> > cell_NodeList(ncm, std::vector<int>(MAX_CELL_PER_NODE, -1));
    std::vector<std::vector<int> > cell_FaceList(ncm, std::vector<int>(MAX_CELL_PER_FACE, -1));
    
    std::vector<int>	                node_ID(nnm,-1);
    std::vector<std::vector<double> >	node_XY(nnm, std::vector<double>(dim, 0.0));
    
    std::vector <int> BCZone_type(nbc, -1);
    std::vector <int> BCZone_nele(nbc, -1);
    
    mesh_file_SU2.clear();
    mesh_file_SU2.seekg(0, std::ios::beg);
    
    
    while (! mesh_file_SU2.eof())
    {
        getline(mesh_file_SU2, line);
        
        // CELL
        readIntTemp = -1;
        readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NELEM=");
        if (readIntTemp != -1)
        {
            int temp_cellType;
            int temp_cellNode[8];
            int temp_cellID;
            
            for (int c=0; c <= ncm-1; c++)
            {
                
                getline(mesh_file_SU2, line);
                sscanf_s(line.c_str(), "%d", &temp_cellType);
                
                switch(temp_cellType)
                {
                    case TRI3:
                        sscanf_s(line.c_str(), "%d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                        
                    case QUAD4:
                        sscanf_s(line.c_str(), "%d %d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellNode[3],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_NodeList[temp_cellID][3] = temp_cellNode[3];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                        
                    case TETRA4:
                        sscanf_s(line.c_str(), "%d %d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellNode[3],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_NodeList[temp_cellID][3] = temp_cellNode[3];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                        
                    case HEXA8:
                        sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellNode[3],
                               &temp_cellNode[4],
                               &temp_cellNode[5],
                               &temp_cellNode[6],
                               &temp_cellNode[7],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_NodeList[temp_cellID][3] = temp_cellNode[3];
                        cell_NodeList[temp_cellID][4] = temp_cellNode[4];
                        cell_NodeList[temp_cellID][5] = temp_cellNode[5];
                        cell_NodeList[temp_cellID][6] = temp_cellNode[6];
                        cell_NodeList[temp_cellID][7] = temp_cellNode[7];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                        
                    case PRISM6:
                        sscanf_s(line.c_str(), "%d %d %d %d %d %d %d %d", &temp_dummy,
                               &temp_cellNode[0],
                               &temp_cellNode[1],
                               &temp_cellNode[2],
                               &temp_cellNode[3],
                               &temp_cellNode[4],
                               &temp_cellNode[5],
                               &temp_cellID);
                        cell_type[temp_cellID]        = temp_dummy;
                        cell_NodeList[temp_cellID][0] = temp_cellNode[0];
                        cell_NodeList[temp_cellID][1] = temp_cellNode[1];
                        cell_NodeList[temp_cellID][2] = temp_cellNode[2];
                        cell_NodeList[temp_cellID][3] = temp_cellNode[3];
                        cell_NodeList[temp_cellID][4] = temp_cellNode[4];
                        cell_NodeList[temp_cellID][5] = temp_cellNode[5];
                        cell_ID[temp_cellID]          = temp_cellID;
                        break;
                }
            }
        }
        
        
        // NODE
        readIntTemp = -1;
        readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NPOIN=");
        if (readIntTemp != -1)
        {
            int temp_nodeID;
            double tempX[3];
            
            if (dim == 2)
            {
                for (int n=0; n <= nnm-1; n++)
                {
                    getline(mesh_file_SU2, line);
                    sscanf_s(line.c_str(), "%lf %lf %d", &tempX[0], &tempX[1], &temp_nodeID);
                    
                    node_XY[temp_nodeID][0] = tempX[0];
                    node_XY[temp_nodeID][1] = tempX[1];
                    node_ID[temp_nodeID]	= temp_nodeID;
                }
            }
            else if (dim == 3)
            {
                for (int n=0; n <= nnm-1; n++)
                {
                    getline(mesh_file_SU2, line);
                    sscanf_s(line.c_str(), "%lf %lf %lf %d", &tempX[0], &tempX[1], &tempX[2], &temp_nodeID);
                    
                    node_XY[temp_nodeID][0] = tempX[0];
                    node_XY[temp_nodeID][1] = tempX[1];
                    node_XY[temp_nodeID][2] = tempX[2];
                    node_ID[temp_nodeID]	= temp_nodeID;
                }
            }
        }
        
        // BCs
        readIntTemp = -1;
        readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NMARK=");
        if (readIntTemp != -1)
        {
            for (int bc = 0; bc <= nbc-1; bc++)
            {
                getline(mesh_file_SU2, line);
                BCZone_type[bc] = Common::read_data_from_string::read_numeric<int>(line, "MARKER_TAG= bc-");
                
                getline(mesh_file_SU2, line);
                BCZone_nele[bc] = Common::read_data_from_string::read_numeric<int>(line, "MARKER_ELEMS=");
                
                for (int f = 0; f <= BCZone_nele[bc]-1; f++)
                {
                    getline(mesh_file_SU2, line);
                }
            }
        }
    }
    
    
    
    // 4. Estimate max nfm
    nfm = 0;
    for (int c = 0; c <= ncm-1; c++)
    {
        switch (cell_type[c])
        {
            case TRI3:
                nfm += 3;
                break;
                
            case QUAD4:
                nfm += 4;
                break;
                
            case TETRA4:
                nfm += 4;
                break;
                
            case HEXA8:
                nfm += 6;
                break;
                
            case PRISM6:
                nfm += 5;
                break;
        }
    }
    
    
    // 5. Construct faces
    std::vector<int>               face_type (nfm, -1);
    std::vector<int>               face_ID(nfm, -1);
    std::vector<int>               face_ID_real(nfm, -1);
    std::vector<int>               face_BC(nfm, 0);
    std::vector<bool>              face_include(nfm, true);
    std::vector<std::vector<int> > face_stencil(nfm, std::vector<int>(2, -1));
    std::vector<std::vector<int> > face_node(nfm, std::vector<int>(MAX_FACE_PER_NODE, -1));
    
    nfm = 0;
    for (int c = 0; c <= ncm-1; c++)
    {
        switch (cell_type[c])
        {
            case TRI3:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][2];
                face_node[nfm][1]    = cell_NodeList[c][0];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][2]	 = nfm;
                nfm++;
                break;
                
            case QUAD4:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][2];
                face_node[nfm][1]    = cell_NodeList[c][3];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][2]	 = nfm;
                nfm++;
                
                // 4rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = LINE;
                face_node[nfm][0]    = cell_NodeList[c][3];
                face_node[nfm][1]    = cell_NodeList[c][0];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][3]	 = nfm;
                nfm++;
                break;
                
            case TETRA4:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_node[nfm][2]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][3];
                face_node[nfm][2]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][3];
                face_node[nfm][2]    = cell_NodeList[c][4];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][2]	 = nfm;
                nfm++;
                
                // 4th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][2];
                face_node[nfm][2]    = cell_NodeList[c][3];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][3]	 = nfm;
                nfm++;
                break;
                
            case HEXA8:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_node[nfm][2]    = cell_NodeList[c][2];
                face_node[nfm][3]    = cell_NodeList[c][3];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][4];
                face_node[nfm][1]    = cell_NodeList[c][7];
                face_node[nfm][2]    = cell_NodeList[c][6];
                face_node[nfm][3]    = cell_NodeList[c][5];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][4];
                face_node[nfm][2]    = cell_NodeList[c][5];
                face_node[nfm][3]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][3]	 = nfm;
                nfm++;
                
                // 4th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][5];
                face_node[nfm][2]    = cell_NodeList[c][6];
                face_node[nfm][3]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][4]	 = nfm;
                nfm++;
                
                // 5th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][2];
                face_node[nfm][1]    = cell_NodeList[c][6];
                face_node[nfm][2]    = cell_NodeList[c][7];
                face_node[nfm][3]    = cell_NodeList[c][3];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][4]	 = nfm;
                nfm++;
                
                // 6th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][3];
                face_node[nfm][1]    = cell_NodeList[c][7];
                face_node[nfm][2]    = cell_NodeList[c][4];
                face_node[nfm][3]    = cell_NodeList[c][0];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][5]	 = nfm;
                nfm++;
                break;
                
            case PRISM6:
                // 1st face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][1];
                face_node[nfm][2]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][0]	 = nfm;
                nfm++;
                
                // 2nd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = TRI3;
                face_node[nfm][0]    = cell_NodeList[c][3];
                face_node[nfm][1]    = cell_NodeList[c][5];
                face_node[nfm][2]    = cell_NodeList[c][4];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][1]	 = nfm;
                nfm++;
                
                // 3rd face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][0];
                face_node[nfm][1]    = cell_NodeList[c][3];
                face_node[nfm][2]    = cell_NodeList[c][4];
                face_node[nfm][3]    = cell_NodeList[c][1];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][2]	 = nfm;
                nfm++;
                
                // 4th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][1];
                face_node[nfm][1]    = cell_NodeList[c][4];
                face_node[nfm][2]    = cell_NodeList[c][5];
                face_node[nfm][3]    = cell_NodeList[c][2];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][3]	 = nfm;
                nfm++;
                
                // 5th face
                face_ID[nfm]         = nfm;
                face_type[nfm]       = QUAD4;
                face_node[nfm][0]    = cell_NodeList[c][2];
                face_node[nfm][1]    = cell_NodeList[c][5];
                face_node[nfm][2]    = cell_NodeList[c][3];
                face_node[nfm][3]    = cell_NodeList[c][0];
                face_stencil[nfm][0] = c;
                face_stencil[nfm][1] = -1;
                
                cell_FaceList[c][4]	 = nfm;
                nfm++;
        }
    }
    
    
    // 6. Remove duplicated faces
    for (int f = 0; f <= nfm-1; f++)
    {
        if (face_include[f] == true)
        {
            for (int f1 = 0; f1 <= nfm-1; f1++)
            {
                if (f1 != f && face_include[f1] == true && face_type[f] == face_type[f1])
                {
                    switch (faceCompare(face_node[f], face_node[f1], face_type[f]))
                    {
                        case 1:
                            face_include[f1] = false;
                            face_ID[f1]  	 = f;
                            break;
                            
                        case -1:
                            face_include[f1] = false;
                            face_ID[f1]      = f;
                            face_stencil[f][1] = face_stencil[f1][0];
                            break;
                    }
                }
            }
        }
    }
    
    
    // 3. Set BC variables and 3rd read
    mesh_file_SU2.clear();
    mesh_file_SU2.seekg(0, std::ios::beg);
    
    while (! mesh_file_SU2.eof())
    {
        getline(mesh_file_SU2, line);
        // BCs
        readIntTemp = -1;
        readIntTemp = Common::read_data_from_string::read_numeric<int>(line, "NMARK=");
        if (readIntTemp != -1)
        {
            for (int bc = 0; bc <= nbc-1; bc++)
            {
                getline(mesh_file_SU2, line);
                BCZone_type[bc] = Common::read_data_from_string::read_numeric<int>(line, "MARKER_TAG= bc-");
                
                getline(mesh_file_SU2, line);
                BCZone_nele[bc] = Common::read_data_from_string::read_numeric<int>(line, "MARKER_ELEMS=");
                
                for (int f = 0; f <= BCZone_nele[bc]-1; f++)
                {
                    int temp_BC_EleType;
                    std::vector<int> temp_BC_EleNodeList(MAX_FACE_PER_NODE);
                    
                    getline(mesh_file_SU2, line);
                    sscanf_s(line.c_str(), "%d", &temp_BC_EleType);
                    
                    switch(temp_BC_EleType)
                    {
                        case LINE:
                            sscanf_s(line.c_str(), "%d %d %d", &temp_dummy, &temp_BC_EleNodeList[0],  &temp_BC_EleNodeList[1]);
                            break;
                            
                        case TRI3:
                            sscanf_s(line.c_str(), "%d %d %d %d", &temp_dummy,
                                   &temp_BC_EleNodeList[0],
                                   &temp_BC_EleNodeList[1],
                                   &temp_BC_EleNodeList[2]);
                            break;
                            
                        case QUAD4:
                            sscanf_s(line.c_str(), "%d %d %d %d %d", &temp_dummy,
                                   &temp_BC_EleNodeList[0],
                                   &temp_BC_EleNodeList[1],
                                   &temp_BC_EleNodeList[2],
                                   &temp_BC_EleNodeList[3]);
                            break;
                    }
                    
                    
                    for (int f1 = 0; f1 <= nfm-1; f1++)
                    {
                        if (face_include[f1] == true && face_type[f1] == temp_dummy)
                        {
                            if (faceCompare(face_node[f1], temp_BC_EleNodeList, temp_dummy) != 0)
                            {
                                face_BC[f1] = BCZone_type[bc];
                            }
                        }
                    }
                }
            }
        }
    }
    
    
    mesh_file_SU2.close();
    
    
    // Update real Face IDs
    int nfm_real = 0;
    int temp_ID;
    
    for (int f = 0; f <= nfm-1; f++)
    {
        if (face_BC[f] == 0)
        {
            if (face_include[f] == true)
            {
                temp_ID = face_ID[f];
                face_ID_real[temp_ID] = nfm_real;
                nfm_real++;
            }
        }
    }
    
    
    for (int f = 0; f <= nfm-1; f++)
    {
        if (face_BC[f] != 0)
        {
            if (face_include[f] == true)
            {
                temp_ID = face_ID[f];
                face_ID_real[temp_ID] = nfm_real;
                nfm_real++;
            }
        }
    }
    
    
    for (int f = 0; f <= nfm-1; f++)
    {
        temp_ID = face_ID[f];
        if (face_ID_real[f] == -1)	face_ID_real[f] = face_ID_real[temp_ID];
    }
    
    
    std::vector<int> face_RealID_to_Pos (nfm_real, -1);
    for (int f = 0; f <= nfm-1; f++)
    {
        temp_ID = face_ID_real[f];
        if (face_RealID_to_Pos[temp_ID] == -1) face_RealID_to_Pos[temp_ID] = f;
    }
    
    
    for (int c = 0; c <= ncm-1; c++)
    {
        int nface;
        switch (cell_type[c])
        {
            case TRI3:
                nface = 3;
                break;
                
            case QUAD4:
                nface = 4;
                break;
                
            case TETRA4:
                nface = 4;
                break;
                
            case HEXA8:
                nface = 6;
                break;
                
            case PRISM6:
                nface = 5;
                break;
                
        }
        
        for (int f = 0; f <= nface-1; f++)
        {
            int tempID = face_ID[cell_FaceList[c][f]];
            cell_FaceList[c][f] = face_ID_real[tempID];
        }
    }
    
    
    // WRITE OP2A GRID DATA
    std::string fileName;
    fileName = out_file_name + ".op2";
    
    std::ofstream mesh_file_OP2;
    mesh_file_OP2.open(fileName.c_str());
    
    
    
    
    mesh_file_OP2 << "%*****************************************************" << std::endl;
    mesh_file_OP2 << "%     OP2A Grid File using FORMAT version 1.0         " << std::endl;
    mesh_file_OP2 << "%                                                     " << std::endl;
    mesh_file_OP2 << "%                  Originally developed by Minkwan Kim" << std::endl;
    mesh_file_OP2 << "%=====================================================" << std::endl;
    mesh_file_OP2 << "% [FACE / CELL TYPE identifiers]                      " << std::endl;
    mesh_file_OP2 << "%      Line                   :  3                    " << std::endl;
    mesh_file_OP2 << "%      Triangle (3-nodes)     :  5                    " << std::endl;
    mesh_file_OP2 << "%      Quadrilateral (4-nodes):  9                    " << std::endl;
    mesh_file_OP2 << "%      Tetrahedral (4-nodes)  : 10                    " << std::endl;
    mesh_file_OP2 << "%      Hexahedral (8-nodes)   : 12                    " << std::endl;
    mesh_file_OP2 << "%      Prism (6-nodes)        : 13                    " << std::endl;
    mesh_file_OP2 << "%      Pramid (5-nodes)       : 14                    " << std::endl;
    mesh_file_OP2 << "%                                                     " << std::endl;
    mesh_file_OP2 << "% [BC TYPE identifiers]                               " << std::endl;
    mesh_file_OP2 << "%      Interior               :  0                    " << std::endl;
    mesh_file_OP2 << "%      Wall                   : 10                    " << std::endl;
    mesh_file_OP2 << "%      Inlet                  : 20                    " << std::endl;
    mesh_file_OP2 << "%      Outlet                 : 30                    " << std::endl;
    mesh_file_OP2 << "%      Freestream (Far-field) : 40                    " << std::endl;
    mesh_file_OP2 << "%      Symmetric              : 50                    " << std::endl;
    mesh_file_OP2 << "%      Axisymmetric           : 60                    " << std::endl;
    mesh_file_OP2 << "%      Anode                  : 70                    " << std::endl;
    mesh_file_OP2 << "%      Cathode                : 80                    " << std::endl;
    mesh_file_OP2 << "%      Dielectric material    : 90                    " << std::endl;
    mesh_file_OP2 << "%                                                     " << std::endl;
    mesh_file_OP2 << "% [DATA Format]                                       " << std::endl;
    mesh_file_OP2 << "%    -Node Data FORMAT                                " << std::endl;
    mesh_file_OP2 << "%     (NODE ID#)  (POSITION DATA: X Y Z)           " << std::endl;
    mesh_file_OP2 << "%    -Face Data FORMAT                                " << std::endl;
    mesh_file_OP2 << "%     (FACE ID#)  (BOUNDARY CONDITION)  (FACE TYPE)  (LIST OF NODE IDs) (LEFT-CELL ID) (RIGHT-CELL ID)" << std::endl;
    mesh_file_OP2 << "%    -Cell Data FORMAT" << std::endl;
    mesh_file_OP2 << "%     (CELL ID#)  (BOUNDARY CONDITION)  (CELL TYPE)  (LIST OF NODE IDs) (LIST OF FACE IDs)" << std::endl;
    mesh_file_OP2 << "%=====================================================" << std::endl;
    mesh_file_OP2 << "%                   Last Format Updated on April/2016 " << std::endl;
    mesh_file_OP2 << "%                                       by M Kim      " << std::endl;
    mesh_file_OP2 << "%*****************************************************" << std::endl << std::endl;
    
    mesh_file_OP2 << "% OVERALL GRID INFORMATION" << std::endl;
    mesh_file_OP2 << "DIM= " << dim << std::endl;
    mesh_file_OP2 << "NNM= " << nnm << std::endl;
    mesh_file_OP2 << "NFM= " << nfm_real << std::endl;
    mesh_file_OP2 << "NCM= " << ncm << std::endl;
    mesh_file_OP2 << std::endl;
    
    mesh_file_OP2 << "%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "NODEDATA= " << nnm << std::endl;
    if (dim == 2)
    {
        double z = 0.0;
        for (int n = 0; n <= nnm-1; n++)
            mesh_file_OP2 << std::setw(8) << node_ID[n]+1 <<
            std::setw(25) << std::scientific << std::setprecision(16) << node_XY[n][0] <<
            std::setw(25) << node_XY[n][1] <<
            std::setw(25) << z << std::endl;
    }
    else
    {
        for (int n = 0; n <= nnm-1; n++)
            mesh_file_OP2 << std::setw(8) << node_ID[n]+1<<
            std::setw(25) << std::scientific << std::setprecision(16) << node_XY[n][0] <<
            std::setw(25) << node_XY[n][1] <<
            std::setw(25) << node_XY[n][2] << std::endl;
    }
    mesh_file_OP2 << std::endl;
    
    mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "FACEDATA= " << nfm_real << std::endl;
    for (int f1 = 0; f1 <= nfm_real-1; f1++)
    {
        int f = face_RealID_to_Pos[f1];
        
        int nnode;
        switch (face_type[f])
        {
            case LINE:
                nnode = 2;
                break;
                
            case TRI3:
                nnode = 3;
                break;
                
            case QUAD4:
                nnode = 4;
                break;
        }
        
        mesh_file_OP2 << std::setw(8) << face_ID_real[f]+1;
        mesh_file_OP2 << std::setw(8) << face_BC[f];
        mesh_file_OP2 << std::setw(8) << face_type[f];
        
        for (int n = 0; n <= nnode-1; n++) mesh_file_OP2 << std::setw(8) << face_node[f][n] + 1;
        mesh_file_OP2 << std::setw(8) << face_stencil[f][0] + 1;
        mesh_file_OP2 << std::setw(8) << face_stencil[f][1] + 1;
        mesh_file_OP2 << std::endl;
    }
    mesh_file_OP2 << std::endl;
    
    
    
    mesh_file_OP2 << "%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "CELLDATA= " << ncm << std::endl;
    for (int c = 0; c <= ncm-1; c++)
    {
        int cellBC = 0;	// Todo: Need to improve later
        
        int nface;
        int nnode;
        
        switch (cell_type[c])
        {
            case TRI3:
                nface = 3;
                nnode = 3;
                break;
                
            case QUAD4:
                nface = 4;
                nnode = 4;
                break;
                
            case TETRA4:
                nface = 4;
                nnode = 4;
                break;
                
            case HEXA8:
                nface = 6;
                nnode = 8;
                break;
                
            case PRISM6:
                nface = 5;
                nnode = 6;
                break;
        }
        
        mesh_file_OP2 << std::setw(8) << cell_ID[c]+1;
        mesh_file_OP2 << std::setw(8) << cellBC;
        mesh_file_OP2 << std::setw(8) << cell_type[c];
        for (int n = 0; n <= nnode-1; n++) mesh_file_OP2 << std::setw(8) << cell_NodeList[c][n] + 1;
        for (int f = 0; f <= nface-1; f++) mesh_file_OP2 << std::setw(8) << cell_FaceList[c][f] + 1;
        mesh_file_OP2 << std::endl;
    }
    mesh_file_OP2 << std::endl;
    mesh_file_OP2.close();
}

