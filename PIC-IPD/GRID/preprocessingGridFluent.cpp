//
//  preprocessingGridFluent.cpp
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
#include "../COMM/VectorCompare.hpp"


int cellTypeConverterFluent(int typeFluent)
{
    int type;
    
    switch (typeFluent)
    {
        case 0:
            type = -1;
            break;
        case 1:
            type = TRI3;
            break;
        case 2:
            type = TETRA4;
            break;
        case 3:
            type = QUAD4;
            break;
        case 4:
            type = HEXA8;
            break;
        case 5:
            type = PYRAMID5;
            break;
        case 6:
            type = PRISM6;
            break;
    }
    
    return (type);
}


void precessingGridFLUENT(const  std::string& mesh_file_name_Fluent)
{
    int index;
    int index_s, index_e;
    int temp_dummy;
    int zone;
    
    std::string line;
    
    // 1. Open file to read
    std::ifstream mesh_file_Fluent;
    mesh_file_Fluent.open(mesh_file_name_Fluent.c_str());
    
    if (!mesh_file_Fluent.is_open()) Common::ExceptionError(FromHere(), "Cannot find grid file. Please check your grid file!", Common::ErrorCodes::FileSystem());
    
    
    // 2. 1st READ
    int dim;
    int nnm;
    int nfm;
    int ncm;
    int nbc;
    int maxZone = 0;
    
    
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        switch(index)
        {
            case 2:
                dim = 0;
                sscanf_s(line.c_str(), "(%d %d)", &temp_dummy, &dim);
                break;
                
            case 10:
                index_s = 0;
                index_e = 0;
                sscanf_s(line.c_str(),"(%d (%x %x %x", &temp_dummy, &zone, &index_s, &index_e); // GETTING INDEX AND ZONE
                if (zone == 0) // READ GENERAL INFORMATION OF NODES
                {
                    nnm = index_e - index_s + 1;
                }
                break;
                
            case 12:
                index_s = 0;
                index_e = 0;
                sscanf_s(line.c_str(),"(%d (%x %x %x", &temp_dummy, &zone, &index_s, &index_e); // GETTING INDEX AND ZONE
                if (zone == 0) // READ GENERAL INFORMATION OF CELLS
                {
                    ncm = index_e - index_s + 1;
                }
                break;
                
            case 13:
                index_s = 0;
                index_e = 0;
                sscanf_s(line.c_str(),"(%d (%x %x %x", &temp_dummy, &zone, &index_s, &index_e); // GETTING INDEX AND ZONE
                
                if (zone == 0) // READ GENERAL INFORMATION OF FACES
                {
                    nfm = index_e - index_s + 1;
                }
                else
                {
                    maxZone = fmax(maxZone, zone);
                }
                break;
        }
    }
    
    // 2. Second Read
    std::vector<int>	nfmZone(maxZone+1, 0);
    std::vector<int>    bcZone(maxZone+1, -1);
    
    
    mesh_file_Fluent.clear();
    mesh_file_Fluent.seekg(0, std::ios::beg);
    
    int temp_zone;
    int bc_type;
    int nfm_zone;
    int num_zone = 0;
    
    int numWall       = 0;
    int numInlet      = 0;
    int numAnode      = 0;
    int numCathode    = 0;
    int numDielectric = 0;
    int numFreestream = 0;
    
    
    char zone_type[20];
    
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        switch(index)
        {
            case 13:
                index_s = 0;
                index_e = 0;
                sscanf_s(line.c_str(),"(%d (%x %x %x", &temp_dummy, &zone, &index_s, &index_e); // GETTING INDEX AND ZONE
                
                if (zone != 0)
                {
                    sscanf_s(line.c_str(),"(%d (%x %x %x %d", &temp_dummy, &temp_zone, &index_s, &index_e, &bc_type); // GETTING INDEX AND ZONE
                    nfm_zone 		= index_e - index_s + 1;
                    nfmZone[zone]	= nfm_zone;
                    num_zone++;
                }
                break;
                
                
            case 45:
                sscanf_s(line.c_str(),"(%d (%d %s", &index, &zone, zone_type); // GETTING INDEX AND ZONE
                
                // GENERAL TYPE OF BOUNDARY CONDITIONS
                bool flagFindBCtype = false;
                
                if (strcmp(zone_type,"interior") == 0)
                {
                    bcZone[zone] = 0;
                    flagFindBCtype = true;
                }
                
                if (strcmp(zone_type,"wall") == 0)
                {
                    if (numWall < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_WALL + numWall;
                        flagFindBCtype = true;
                        numWall++;
                    }
                }
                
                if (strcmp(zone_type,"pressure-inlet") == 0)
                {
                    if (numInlet < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_INLET + numInlet;
                        flagFindBCtype = true;
                        numInlet++;
                    }
                }
                
                if (strcmp(zone_type,"pressure-outlet") == 0)
                {
                    bcZone[zone] = BC_OUTLET;
                    flagFindBCtype = true;
                }
                
                if (strcmp(zone_type,"pressure-far-field") == 0)
                {
                    if (numFreestream < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_FREESTREAM + numFreestream;
                        flagFindBCtype = true;
                        numFreestream++;
                    }
                }
                
                if (strcmp(zone_type,"symmetry") == 0)
                {
                    bcZone[zone] = BC_SYMMETRY;
                    flagFindBCtype = true;
                }
                
                if (strcmp(zone_type,"axis") == 0)
                {
                    bcZone[zone] = BC_AXIS;
                    flagFindBCtype = true;
                }
                
                if (strcmp(zone_type,"anode") == 0)
                {
                    if (numAnode < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_ANODE + numAnode;
                        flagFindBCtype = true;
                        numAnode++;
                    }
                }
                
                if (strcmp(zone_type,"cathode") == 0)
                {
                    if (numCathode < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_CATHODE + numCathode;
                        flagFindBCtype = true;
                        numCathode++;
                    }
                }
                
                if (strcmp(zone_type,"dielectric-wall") == 0)
                {
                    if (numDielectric < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_DIELECTRIC + numDielectric;
                        flagFindBCtype = true;
                        numDielectric++;
                    }
                }
                
                if (strcmp(zone_type,"fluid") == 0)
                {
                    bcZone[zone] = 0;
                    flagFindBCtype = true;
                }
                
                if (flagFindBCtype == false)
                {
                    Common::ExceptionError(FromHere(), "Given BC type in the grid is not supported BC type. Please check BC type in the grid file", Common::ErrorCodes::NotSupportedType());
                }
                break;
        }
    }
    
    
    // 3. Read Node Data (3rd READ)
    std::vector<int>	                node_ID(nnm,-1);
    std::vector<int>	                node_whereis(nnm+1, -1);
    std::vector<std::vector<double> >	node_XY(nnm, std::vector<double>(dim, 0.0));
    mesh_file_Fluent.clear();
    mesh_file_Fluent.seekg(0, std::ios::beg);
    
    
    int type;
    int temp_dim;
    int n = 0;
    double x, y, z;
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        if (index == 10)
        {
            temp_dummy = -1;
            zone       = -1;
            index_s    = -1;
            index_e    = -1;
            type       = -1;
            temp_dim   = -1;
            
            sscanf_s(line.c_str(),"(%d (%x %x %x %d %d)", &temp_dummy, &zone, &index_s, &index_e, &type, &temp_dim); // GETTING INDEX AND ZONE
            if (zone > 0)
            {
                if (temp_dim != dim)
                    Common::ExceptionError(FromHere(), "PROBLEM IN DIMENSION OF NODE DATA. Please the dimension of grid file", Common::ErrorCodes::MismatchData());
                
                if (dim == 2)
                {
                    for (int i = index_s; i <= index_e; i++)
                    {
                        x = 0.0;
                        y = 0.0;
                        
                        getline(mesh_file_Fluent, line);
                        sscanf_s(line.c_str(), "%lf %lf", &x, &y);
                        
                        node_ID[n]    = n;
                        node_XY[n][0] = x;
                        node_XY[n][1] = y;
                        node_whereis[i] = n;
                        n++;
                    }
                }
                else
                {
                    for (int i = index_s; i <= index_e; i++)
                    {
                        x = 0.0;
                        y = 0.0;
                        z = 0.0;
                        
                        getline(mesh_file_Fluent, line);
                        sscanf_s(line.c_str(), "%lf %lf %lf", &x, &y, &z);
                        
                        node_ID[n]    = i;
                        node_XY[n][0] = x;
                        node_XY[n][1] = y;
                        node_XY[n][2] = z;
                        node_whereis[i] = n;
                        n++;
                    }
                }
            }
        }
    }
    
    if (n != nnm) Common::ExceptionError(FromHere(), "PROBLEM IN NODE DATA. Number of read node does not match with header", Common::ErrorCodes::MismatchData());
    // END READ NODE DATA
    
    
    
    // 4. Read Face Data (4th READ)
    std::vector<int>               face_type (nfm, -1);
    std::vector<int>               face_ID(nfm, -1);
    std::vector<int>               face_whereis(nfm+1, -1);
    std::vector<int>               face_BC(nfm, 0);
    std::vector<std::vector<int> > face_stencil(nfm, std::vector<int>(2, -1));
    std::vector<std::vector<int> > face_node(nfm, std::vector<int>(MAX_FACE_PER_NODE, -1));
    
    int f = 0;
    
    mesh_file_Fluent.clear();
    mesh_file_Fluent.seekg(0, std::ios::beg);
    
    
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        if (index == 13)
        {
            temp_dummy	= -1;
            zone 		= -1;
            index_s     = -1;
            index_e     = -1;
            bc_type     = -1;
            type 	    = -1;
            
            sscanf_s(line.c_str(),"(%d (%x %x %x %d %d)", &temp_dummy, &zone, &index_s, &index_e, &bc_type, &type); // GETTING INDEX AND ZONE
            
            if (zone > 0)
            {
                // Assign BC type from data
                bc_type	= bcZone[zone];
                
                switch (type)
                {
                    case 0:
                        for (int i = index_s; i <= index_e; i++)
                        {
                            int temp_faceType = -1;
                            getline(mesh_file_Fluent, line);
                            sscanf_s(line.c_str(), "%d", &temp_faceType);
                            
                            std::vector<int> temp_nodeList (MAX_FACE_PER_NODE, -1);
                            int cl = -1;
                            int cr = -1;
                            
                            switch (temp_faceType)
                            {
                                case 2: // LINE
                                    sscanf_s(line.c_str(), "%d %x %x %x %x", &type, &temp_nodeList[0], &temp_nodeList[1], &cl, &cr);
                                    
                                    face_ID[f]         = i;
                                    face_type[f]       = LINE;
                                    face_node[f][0]    = temp_nodeList[0];
                                    face_node[f][1]    = temp_nodeList[1];
                                    face_stencil[f][0] = cl;
                                    face_stencil[f][1] = cr;
                                    break;
                                    
                                case 3: // Triangle
                                    sscanf_s(line.c_str(), "%d %x %x %x %x %x", &type, &temp_nodeList[0], &temp_nodeList[1], &temp_nodeList[2], &cl, &cr);
                                    
                                    face_ID[f]         = i;
                                    face_type[f]       = TRI3;
                                    face_node[f][0]    = temp_nodeList[0];
                                    face_node[f][1]    = temp_nodeList[1];
                                    face_node[f][2]    = temp_nodeList[2];
                                    face_stencil[f][0] = cl;
                                    face_stencil[f][1] = cr;
                                    break;
                                    
                                case 4: // Quadrilateral
                                    sscanf_s(line.c_str(), "%d %x %x %x %x %x %x", &type, &temp_nodeList[0], &temp_nodeList[1], &temp_nodeList[2], &temp_nodeList[3], &cl, &cr);
                                    
                                    face_ID[f]         = i;
                                    face_type[f]       = QUAD4;
                                    face_node[f][0]    = temp_nodeList[0];
                                    face_node[f][1]    = temp_nodeList[1];
                                    face_node[f][2]    = temp_nodeList[2];
                                    face_node[f][3]    = temp_nodeList[3];
                                    face_stencil[f][0] = cl;
                                    face_stencil[f][1] = cr;
                                    break;
                                    
                                default:
                                    Common::ExceptionError(FromHere(), "It is not supported Face-type. PLEASE CHECK THE FACE TYPE IN A MESH FILE!", Common::ErrorCodes::NotSupportedType());
                                    break;
                            }
                            
                            face_BC[f]      = bc_type;
                            face_whereis[i]	= f;
                            f++;
                        }
                        break;
                        
                    case 2: // LINE
                        for (int i = index_s; i <= index_e; i++)
                        {
                            std::vector<int> temp_nodeList (MAX_FACE_PER_NODE, -1);
                            int cl = -1;
                            int cr = -1;
                            
                            getline(mesh_file_Fluent, line);
                            sscanf_s(line.c_str(), "%x %x %x %x", &temp_nodeList[0], &temp_nodeList[1], &cl, &cr);
                            
                            face_ID[f]         = i;
                            face_type[f]       = LINE;
                            face_node[f][0]    = temp_nodeList[0];
                            face_node[f][1]    = temp_nodeList[1];
                            face_stencil[f][0] = cl;
                            face_stencil[f][1] = cr;
                            
                            face_BC[f]         = bc_type;
                            face_whereis[i]	   = f;
                            f++;
                        }
                        break;
                        
                    case 3: // Triangle
                        for (int i = index_s; i <= index_e; i++)
                        {
                            std::vector<int> temp_nodeList (MAX_FACE_PER_NODE, -1);
                            int cl = -1;
                            int cr = -1;
                            
                            getline(mesh_file_Fluent, line);
                            sscanf_s(line.c_str(), "%x %x %x %x %x", &temp_nodeList[0], &temp_nodeList[1], &temp_nodeList[2], &cl, &cr);
                            
                            face_ID[f]         = i;
                            face_type[f]       = TRI3;
                            face_node[f][0]    = temp_nodeList[0];
                            face_node[f][1]    = temp_nodeList[1];
                            face_node[f][2]    = temp_nodeList[2];
                            face_stencil[f][0] = cl;
                            face_stencil[f][1] = cr;
                            
                            face_BC[f]         = bc_type;
                            face_whereis[i]	   = f;
                            f++;
                        }
                        break;
                        
                    case 4: // Quadrilateral
                        for (int i = index_s; i <= index_e; i++)
                        {
                            std::vector<int> temp_nodeList (MAX_FACE_PER_NODE, -1);
                            int cl = -1;
                            int cr = -1;
                            
                            getline(mesh_file_Fluent, line);
                            sscanf_s(line.c_str(), "%x %x %x %x %x %x", &temp_nodeList[0], &temp_nodeList[1], &temp_nodeList[2], &temp_nodeList[3], &cl, &cr);
                            
                            face_ID[f]         = i;
                            face_type[f]       = QUAD4;
                            face_node[f][0]    = temp_nodeList[0];
                            face_node[f][1]    = temp_nodeList[1];
                            face_node[f][2]    = temp_nodeList[2];
                            face_node[f][3]    = temp_nodeList[3];
                            face_stencil[f][0] = cl;
                            face_stencil[f][1] = cr;
                            
                            face_BC[f]         = bc_type;
                            face_whereis[i]	   = f;
                            f++;
                        }
                        break;
                        
                    default:
                        Common::ExceptionError(FromHere(), "It is not supported Face-type. PLEASE CHECK THE FACE TYPE IN A MESH FILE!", Common::ErrorCodes::NotSupportedType());
                        break;
                }
            }
        }
    }
    
    if (f != nfm) Common::ExceptionError(FromHere(), "PROBLEM IN Face DATA. Number of read face does not match with header", Common::ErrorCodes::MismatchData());
    
    
    // 5. Read Cell Data (5th READ)
    std::vector<int>               cell_type(ncm, -1);
    std::vector<int>               cell_ID(ncm, -1);
    std::vector<int>               cell_BC(ncm, -1);
    std::vector<int>               cell_whereis(ncm+1, -1);
    std::vector<std::vector<int> > cell_NodeList(ncm, std::vector<int>(MAX_CELL_PER_NODE, -1));
    std::vector<std::vector<int> > cell_FaceList(ncm, std::vector<int>(MAX_CELL_PER_FACE, -1));
    
    int c = 0;
    int temp_type;
    mesh_file_Fluent.clear();
    mesh_file_Fluent.seekg(0, std::ios::beg);
    
    
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        if (index == 12)
        {
            temp_dummy	= -1;
            zone 		= -1;
            index_s		= -1;
            index_e		= -1;
            type 		= -1;
            temp_type	= -1;
            
            sscanf_s(line.c_str(),"(%d (%x %x %x %d", &temp_dummy, &zone, &index_s, &index_e, &type); // GETTING INDEX AND ZONE
            if (zone > 0)
            {
                sscanf_s(line.c_str(),"(%d (%x %x %x %d %d)", &temp_dummy, &zone, &index_e, &index_e, &type, &temp_type); // GETTING INDEX AND ZONE
                
                if (type == 1)
                {
                    if (temp_type != 0)
                    {
                        for (int i = index_s; i <= index_e; i++)
                        {
                            cell_ID[c]      = i;
                            cell_BC[c]      = BC_INTERIOR;
                            cell_type[c]    = cellTypeConverterFluent(temp_type);
                            cell_whereis[i]	= c;
                            c++;
                        }
                    }
                    else
                    {
                        for (int i = index_s; i <= index_e; i++)
                        {
                            mesh_file_Fluent >> temp_type;
                            
                            cell_ID[c]      = i;
                            cell_BC[c]      = BC_INTERIOR;
                            cell_type[c]    = cellTypeConverterFluent(temp_type);
                            cell_whereis[i]	= c;
                            c++;
                        }
                    }
                }
            }
        }
    }
    if (c != ncm) Common::ExceptionError(FromHere(), "PROBLEM IN Cell DATA. Number of read cell does not match with header", Common::ErrorCodes::MismatchData());
    
    
    
    
    
    
    /*
     * Construct cells (Node and Face list)
     */
    int flag;
    std::vector<int> filled_nodes(ncm, 0);
    std::vector<int> filled_faces(ncm, 0);
    
    std::ostringstream error_message;
    
    for (int f = 0; f <= nfm-1; f++)
    {
        int cl	= face_stencil[f][0];
        int cr	= face_stencil[f][1];
        
        // For Right Cell
        if (cr != -1)
        {
            c = cell_whereis[cr];
            
            switch (cell_type[c])
            {
                case TRI3:
                    if (face_type[f] != LINE)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][1];
                        cell_NodeList[c][1]	= face_node[f][0];
                        filled_nodes[c]	= 2;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 2)
                    {
                        if (face_node[f][0] == cell_NodeList[c][0])
                        {
                            cell_NodeList[c][2]	= face_node[f][1];
                            filled_nodes[c]	= 3;
                            
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c]++;
                        }
                        else if (face_node[f][1] == cell_NodeList[c][1])
                        {
                            cell_NodeList[c][2]	= face_node[f][0];
                            filled_nodes[c]	= 3;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c]++;
                        }
                    }
                    else
                    {
                        if (filled_faces[c] == 2)
                        {
                            if (face_node[f][0] == cell_NodeList[c][0] && face_node[f][1] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if (face_node[f][0] == cell_NodeList[c][2] && face_node[f][1] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case QUAD4:
                    if (face_type[f] != LINE)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][1];
                        cell_NodeList[c][1]	= face_node[f][0];
                        filled_nodes[c]	= 2;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 2)
                    {
                        if (face_node[f][0] != cell_NodeList[c][0] &&
                            face_node[f][0] != cell_NodeList[c][1] &&
                            face_node[f][1] != cell_NodeList[c][0] &&
                            face_node[f][1] != cell_NodeList[c][1])
                        {
                            cell_NodeList[c][2]	= face_node[f][1];
                            cell_NodeList[c][3]	= face_node[f][0];
                            filled_nodes[c]	= 4;
                            
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c]++;
                        }
                        
                        if (cell_NodeList[c][0] == face_node[f][0] && cell_NodeList[c][1] != face_node[f][1])
                        {
                            if (cell_FaceList[c][3] == -1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        
                        if (cell_NodeList[c][0] != face_node[f][0] && cell_NodeList[c][1] == face_node[f][1])
                        {
                            if (cell_FaceList[c][1] == -1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    else if (filled_nodes[c] == 4)
                    {
                        if (face_node[f][0] == cell_NodeList[c][0] && face_node[f][1] == cell_NodeList[c][3])
                        {
                            if (cell_FaceList[c][3] == -1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][0] == cell_NodeList[c][2] && face_node[f][1] == cell_NodeList[c][1])
                        {
                            if (cell_FaceList[c][1] == -1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    
                    break;
                    
                case TETRA4:
                    if (face_type[f] != TRI3)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][1];
                        cell_NodeList[c][2]	= face_node[f][2];
                        filled_nodes[c]	= 3;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    if (filled_nodes[c] == 3)
                    {
                        if (face_node[f][0] != cell_NodeList[c][0] &&
                            face_node[f][0] != cell_NodeList[c][1] &&
                            face_node[f][0] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][0];
                            filled_nodes[c]++;
                            
                            if(face_node[f][1] == cell_NodeList[c][0] &&
                               face_node[f][2] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][1] == cell_NodeList[c][1] &&
                                    face_node[f][2] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][1] == cell_NodeList[c][2] &&
                                    face_node[f][2] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][1] != cell_NodeList[c][0] &&
                                 face_node[f][1] != cell_NodeList[c][1] &&
                                 face_node[f][1] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][1];
                            filled_nodes[c]++;
                            
                            if(face_node[f][0] == cell_NodeList[c][0] &&
                               face_node[f][2] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][1] &&
                                    face_node[f][2] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][2] &&
                                    face_node[f][2] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][2] != cell_NodeList[c][0] &&
                                 face_node[f][2] != cell_NodeList[c][1] &&
                                 face_node[f][2] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][2];
                            filled_nodes[c]++;
                            
                            if(face_node[f][0] == cell_NodeList[c][1] &&
                               face_node[f][1] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][2] &&
                                    face_node[f][1] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][0] &&
                                    face_node[f][1] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case HEXA8:
                    if (face_type[f] != QUAD4)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][1];
                        cell_NodeList[c][2]	= face_node[f][2];
                        cell_NodeList[c][3]	= face_node[f][3];
                        filled_nodes[c]	= 4;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 4 || filled_nodes[c] == 6)
                    {
                        if(cell_NodeList[c][0] == face_node[f][0]&&
                           cell_NodeList[c][1] == face_node[f][3])
                        {
                            cell_NodeList[c][4]	= face_node[f][1];
                            cell_NodeList[c][5]	= face_node[f][2];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][1]&&
                                cell_NodeList[c][1] == face_node[f][0])
                        {
                            cell_NodeList[c][4]	= face_node[f][2];
                            cell_NodeList[c][5]	= face_node[f][3];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][2]&&
                                cell_NodeList[c][1] == face_node[f][1])
                        {
                            cell_NodeList[c][4]	= face_node[f][3];
                            cell_NodeList[c][5]	= face_node[f][0];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][3]&&
                                cell_NodeList[c][1] == face_node[f][2])
                        {
                            cell_NodeList[c][4]	= face_node[f][0];
                            cell_NodeList[c][5]	= face_node[f][1];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][0]&&
                                cell_NodeList[c][3] == face_node[f][3])
                        {
                            cell_NodeList[c][6]	= face_node[f][1];
                            cell_NodeList[c][7]	= face_node[f][2];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][1]&&
                                cell_NodeList[c][3] == face_node[f][0])
                        {
                            cell_NodeList[c][6]	= face_node[f][2];
                            cell_NodeList[c][7]	= face_node[f][3];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][2]&&
                                cell_NodeList[c][3] == face_node[f][1])
                        {
                            cell_NodeList[c][6]	= face_node[f][3];
                            cell_NodeList[c][7]	= face_node[f][0];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][3]&&
                                cell_NodeList[c][3] == face_node[f][2])
                        {
                            cell_NodeList[c][6]	= face_node[f][0];
                            cell_NodeList[c][7]	= face_node[f][1];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                    }
                    else
                    {
                        std::vector<int> temp_face2(4);
                        std::vector<int> temp_face4(4);
                        std::vector<int> temp_face5(4);
                        
                        temp_face2[0]	= cell_NodeList[c][1];
                        temp_face2[1]	= cell_NodeList[c][5];
                        temp_face2[2]	= cell_NodeList[c][6];
                        temp_face2[3]	= cell_NodeList[2][2];
                        
                        temp_face4[0]	= cell_NodeList[c][0];
                        temp_face4[1]	= cell_NodeList[c][3];
                        temp_face4[2]	= cell_NodeList[c][7];
                        temp_face4[3]	= cell_NodeList[2][4];
                        
                        temp_face5[0]	= cell_NodeList[c][4];
                        temp_face5[1]	= cell_NodeList[c][7];
                        temp_face5[2]	= cell_NodeList[c][6];
                        temp_face5[3]	= cell_NodeList[2][5];
                        
                        if (faceCompare(temp_face2, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        
                        if (faceCompare(temp_face4, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][4]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        
                        if (faceCompare(temp_face5, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][5]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                    }
                    break;
                    
                case PRISM6:
                    if (face_type[f] == TRI3)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][1];
                            cell_NodeList[c][2]	= face_node[f][2];
                            filled_nodes[c] = 3;
                            
                            cell_FaceList[c][0]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 5)
                        {
                            if (cell_NodeList[c][3] == face_node[f][0] &&
                                cell_NodeList[c][4] == face_node[f][2])
                            {
                                cell_NodeList[c][5]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][1] &&
                                     cell_NodeList[c][4] == face_node[f][0])
                            {
                                cell_NodeList[c][5]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][2] &&
                                     cell_NodeList[c][4] == face_node[f][1])
                            {
                                cell_NodeList[c][5]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else if (face_type[f] == QUAD4)
                    {
                        if (filled_nodes[c] == 3)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][3])
                            {
                                cell_NodeList[c][3]	= face_node[f][1];
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][3]	= face_node[f][2];
                                cell_NodeList[c][4]	= face_node[f][3];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][3]	= face_node[f][3];
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][3] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][3]	= face_node[f][0];
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                        else if (filled_nodes[c] == 6)
                        {
                            std::vector<int> temp_face2(4);
                            std::vector<int> temp_face3(4);
                            
                            temp_face2[0]	= cell_NodeList[c][1];
                            temp_face2[1]	= cell_NodeList[c][4];
                            temp_face2[2]	= cell_NodeList[c][5];
                            temp_face2[3]	= cell_NodeList[2][2];
                            
                            temp_face3[0]	= cell_NodeList[c][0];
                            temp_face3[1]	= cell_NodeList[c][2];
                            temp_face3[2]	= cell_NodeList[c][5];
                            temp_face3[3]	= cell_NodeList[2][3];
                            
                            
                            if (faceCompare(temp_face2, face_node[f], QUAD4) == 1)
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face3, face_node[f], QUAD4) == 1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    break;
                    
                case PYRAMID5:
                    if (face_type[f] == QUAD4)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][1];
                            cell_NodeList[c][2]	= face_node[f][2];
                            cell_NodeList[c][3]	= face_node[f][3];
                            
                            filled_nodes[c] = 4;
                            
                            cell_FaceList[c][0]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 3)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][2]	= face_node[f][2];
                                cell_NodeList[c][3]	= face_node[f][3];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][2]	= face_node[f][3];
                                cell_NodeList[c][3]	= face_node[f][0];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][3])
                            {
                                cell_NodeList[c][2]	= face_node[f][0];
                                cell_NodeList[c][3]	= face_node[f][1];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][3] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][2]	= face_node[f][1];
                                cell_NodeList[c][3]	= face_node[f][2];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else if (face_type[f] == TRI3)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][2];
                            cell_NodeList[c][4]	= face_node[f][1];
                            filled_nodes[c] = 3;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 4)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][0] &&
                                     cell_NodeList[c][2] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][1] &&
                                     cell_NodeList[c][2] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][2] &&
                                     cell_NodeList[c][2] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][0] &&
                                     cell_NodeList[c][3] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][1] &&
                                     cell_NodeList[c][3] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][2] &&
                                     cell_NodeList[c][3] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][0] &&
                                     cell_NodeList[c][0] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][1] &&
                                     cell_NodeList[c][0] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][2] &&
                                     cell_NodeList[c][0] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                        else if (filled_nodes[c] == 5)
                        {
                            std::vector<int> temp_face1(3);
                            std::vector<int> temp_face2(3);
                            std::vector<int> temp_face3(3);
                            std::vector<int> temp_face4(3);
                            
                            temp_face1[0]	= cell_NodeList[c][0];
                            temp_face1[1]	= cell_NodeList[c][4];
                            temp_face1[2]	= cell_NodeList[c][1];
                            
                            temp_face2[0]	= cell_NodeList[c][1];
                            temp_face2[1]	= cell_NodeList[c][4];
                            temp_face2[2]	= cell_NodeList[c][2];
                            
                            temp_face3[0]	= cell_NodeList[c][3];
                            temp_face3[1]	= cell_NodeList[c][2];
                            temp_face3[2]	= cell_NodeList[c][4];
                            
                            temp_face4[0]	= cell_NodeList[c][0];
                            temp_face4[1]	= cell_NodeList[c][3];
                            temp_face4[2]	= cell_NodeList[c][4];
                            
                            
                            if (faceCompare(temp_face1, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face2, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face3, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face4, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    break;
            }
        } // end right-cell
        
        
        
        // For Left Cell
        if (cl != -1)
        {
            c = cell_whereis[cl];
            
            switch (cell_type[c])
            {
                case TRI3:
                    if (face_type[f] != LINE)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][1];
                        filled_nodes[c]	= 2;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 2)
                    {
                        if (face_node[f][0] == cell_NodeList[c][1])
                        {
                            cell_NodeList[c][2]	= face_node[f][1];
                            filled_nodes[c]	= 3;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c]++;
                        }
                        else if (face_node[f][1] == cell_NodeList[c][0])
                        {
                            cell_NodeList[c][2]	= face_node[f][0];
                            filled_nodes[c]	= 3;
                            
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c]++;
                        }
                    }
                    else
                    {
                        if (filled_faces[c] == 2)
                        {
                            if (face_node[f][0] == cell_NodeList[c][1] && face_node[f][1] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if (face_node[f][0] == cell_NodeList[c][2] && face_node[f][1] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case QUAD4:
                    if (face_type[f] != LINE)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][1];
                        filled_nodes[c]	= 2;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 2)
                    {
                        if (face_node[f][0] != cell_NodeList[c][0] &&
                            face_node[f][0] != cell_NodeList[c][1] &&
                            face_node[f][1] != cell_NodeList[c][0] &&
                            face_node[f][1] != cell_NodeList[c][1])
                        {
                            cell_NodeList[c][2]	= face_node[f][0];
                            cell_NodeList[c][3]	= face_node[f][1];
                            filled_nodes[c]	= 4;
                            
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c]++;
                        }
                        
                        if (cell_NodeList[c][0] == face_node[f][1] && cell_NodeList[c][1] != face_node[f][0])
                        {
                            if (cell_FaceList[c][3] == -1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        
                        if (cell_NodeList[c][0] != face_node[f][1] && cell_NodeList[c][1] == face_node[f][0])
                        {
                            if (cell_FaceList[c][1] == -1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    else if (filled_nodes[c] == 4)
                    {
                        if (face_node[f][0] == cell_NodeList[c][3] && face_node[f][1] == cell_NodeList[c][0])
                        {
                            if (cell_FaceList[c][3] == -1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][0] == cell_NodeList[c][1] && face_node[f][1] == cell_NodeList[c][2])
                        {
                            if (cell_FaceList[c][1] == -1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case TETRA4:
                    if (face_type[f] != TRI3)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][2];
                        cell_NodeList[c][2]	= face_node[f][1];
                        filled_nodes[c]	= 3;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    if (filled_nodes[c] == 3)
                    {
                        if (face_node[f][0] != cell_NodeList[c][0] &&
                            face_node[f][0] != cell_NodeList[c][1] &&
                            face_node[f][0] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][0];
                            filled_nodes[c]++;
                            
                            if(face_node[f][1] == cell_NodeList[c][0] &&
                               face_node[f][2] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][1] == cell_NodeList[c][1] &&
                                    face_node[f][2] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][1] == cell_NodeList[c][2] &&
                                    face_node[f][2] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][1] != cell_NodeList[c][0] &&
                                 face_node[f][1] != cell_NodeList[c][1] &&
                                 face_node[f][1] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][1];
                            filled_nodes[c]++;
                            
                            if(face_node[f][0] == cell_NodeList[c][1] &&
                               face_node[f][2] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][2] &&
                                    face_node[f][2] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][0] &&
                                    face_node[f][2] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][2] != cell_NodeList[c][0] &&
                                 face_node[f][2] != cell_NodeList[c][1] &&
                                 face_node[f][2] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][2];
                            filled_nodes[c]++;
                            
                            if(face_node[f][0] == cell_NodeList[c][0] &&
                               face_node[f][1] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][1] &&
                                    face_node[f][1] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][2] &&
                                    face_node[f][1] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case HEXA8:
                    if (face_type[f] != QUAD4)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][3];
                        cell_NodeList[c][2]	= face_node[f][2];
                        cell_NodeList[c][3]	= face_node[f][1];
                        filled_nodes[c]	= 4;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 4 || filled_nodes[c] == 6)
                    {
                        if(cell_NodeList[c][0] == face_node[f][0]&&
                           cell_NodeList[c][1] == face_node[f][1])
                        {
                            cell_NodeList[c][4]	= face_node[f][3];
                            cell_NodeList[c][5]	= face_node[f][2];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][1]&&
                                cell_NodeList[c][1] == face_node[f][2])
                        {
                            cell_NodeList[c][4]	= face_node[f][0];
                            cell_NodeList[c][5]	= face_node[f][3];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][2]&&
                                cell_NodeList[c][1] == face_node[f][3])
                        {
                            cell_NodeList[c][4]	= face_node[f][1];
                            cell_NodeList[c][5]	= face_node[f][0];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][3]&&
                                cell_NodeList[c][1] == face_node[f][0])
                        {
                            cell_NodeList[c][4]	= face_node[f][2];
                            cell_NodeList[c][5]	= face_node[f][1];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][0]&&
                                cell_NodeList[c][3] == face_node[f][1])
                        {
                            cell_NodeList[c][6]	= face_node[f][3];
                            cell_NodeList[c][7]	= face_node[f][2];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][1]&&
                                cell_NodeList[c][3] == face_node[f][2])
                        {
                            cell_NodeList[c][6]	= face_node[f][0];
                            cell_NodeList[c][7]	= face_node[f][3];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][2]&&
                                cell_NodeList[c][3] == face_node[f][3])
                        {
                            cell_NodeList[c][6]	= face_node[f][1];
                            cell_NodeList[c][7]	= face_node[f][0];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][3]&&
                                cell_NodeList[c][3] == face_node[f][0])
                        {
                            cell_NodeList[c][6]	= face_node[f][2];
                            cell_NodeList[c][7]	= face_node[f][1];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                    }
                    else
                    {
                        std::vector<int> temp_face2(4);
                        std::vector<int> temp_face4(4);
                        std::vector<int> temp_face5(4);
                        
                        temp_face2[0]	= cell_NodeList[c][1];
                        temp_face2[1]	= cell_NodeList[c][2];
                        temp_face2[2]	= cell_NodeList[c][6];
                        temp_face2[3]	= cell_NodeList[2][5];
                        
                        temp_face4[0]	= cell_NodeList[c][0];
                        temp_face4[1]	= cell_NodeList[c][4];
                        temp_face4[2]	= cell_NodeList[c][7];
                        temp_face4[3]	= cell_NodeList[2][3];
                        
                        temp_face5[0]	= cell_NodeList[c][4];
                        temp_face5[1]	= cell_NodeList[c][5];
                        temp_face5[2]	= cell_NodeList[c][6];
                        temp_face5[3]	= cell_NodeList[2][7];
                        
                        if (faceCompare(temp_face2, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        
                        if (faceCompare(temp_face4, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][4]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        
                        if (faceCompare(temp_face5, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][5]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                    }
                    break;
                    
                case PRISM6:
                    if (face_type[f] == TRI3)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][2];
                            cell_NodeList[c][2]	= face_node[f][1];
                            filled_nodes[c] = 3;
                            
                            cell_FaceList[c][0]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 5)
                        {
                            if (cell_NodeList[c][3] == face_node[f][0] &&
                                cell_NodeList[c][4] == face_node[f][1])
                            {
                                cell_NodeList[c][5]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][1] &&
                                     cell_NodeList[c][4] == face_node[f][2])
                            {
                                cell_NodeList[c][5]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][2] &&
                                     cell_NodeList[c][4] == face_node[f][0])
                            {
                                cell_NodeList[c][5]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else if (face_type[f] == QUAD4)
                    {
                        if (filled_nodes[c] == 3)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][3]	= face_node[f][3];
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][3]	= face_node[f][0];
                                cell_NodeList[c][4]	= face_node[f][3];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][3])
                            {
                                cell_NodeList[c][3]	= face_node[f][1];
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][3] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][3]	= face_node[f][2];
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                        else if (filled_nodes[c] == 6)
                        {
                            std::vector<int> temp_face2(4);
                            std::vector<int> temp_face3(4);
                            
                            temp_face2[0]	= cell_NodeList[c][1];
                            temp_face2[1]	= cell_NodeList[c][2];
                            temp_face2[2]	= cell_NodeList[c][5];
                            temp_face2[3]	= cell_NodeList[2][4];
                            
                            temp_face3[0]	= cell_NodeList[c][0];
                            temp_face3[1]	= cell_NodeList[c][3];
                            temp_face3[2]	= cell_NodeList[c][5];
                            temp_face3[3]	= cell_NodeList[2][2];
                            
                            
                            if (faceCompare(temp_face2, face_node[f], QUAD4) == 1)
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face3, face_node[f], QUAD4) == 1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    break;
                    
                case PYRAMID5:
                    if (face_type[f] == QUAD4)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][3];
                            cell_NodeList[c][2]	= face_node[f][2];
                            cell_NodeList[c][3]	= face_node[f][1];
                            filled_nodes[c] = 4;
                            
                            cell_FaceList[c][0]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 3)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][3])
                            {
                                cell_NodeList[c][2]	= face_node[f][2];
                                cell_NodeList[c][3]	= face_node[f][1];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][2]	= face_node[f][3];
                                cell_NodeList[c][3]	= face_node[f][2];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][2]	= face_node[f][0];
                                cell_NodeList[c][3]	= face_node[f][3];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][3] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][2]	= face_node[f][1];
                                cell_NodeList[c][3]	= face_node[f][0];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else if (face_type[f] == TRI3)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][1];
                            cell_NodeList[c][4]	= face_node[f][2];
                            filled_nodes[c] = 3;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 4)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][0] &&
                                     cell_NodeList[c][2] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][1] &&
                                     cell_NodeList[c][2] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][2] &&
                                     cell_NodeList[c][2] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][0] &&
                                     cell_NodeList[c][3] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][1] &&
                                     cell_NodeList[c][3] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][2] &&
                                     cell_NodeList[c][3] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][0] &&
                                     cell_NodeList[c][0] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][1] &&
                                     cell_NodeList[c][0] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][2] &&
                                     cell_NodeList[c][0] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                        else if (filled_nodes[c] == 5)
                        {
                            std::vector<int> temp_face1(3);
                            std::vector<int> temp_face2(3);
                            std::vector<int> temp_face3(3);
                            std::vector<int> temp_face4(3);
                            
                            temp_face1[0]	= cell_NodeList[c][0];
                            temp_face1[1]	= cell_NodeList[c][1];
                            temp_face1[2]	= cell_NodeList[c][4];
                            
                            temp_face2[0]	= cell_NodeList[c][1];
                            temp_face2[1]	= cell_NodeList[c][2];
                            temp_face2[2]	= cell_NodeList[c][4];
                            
                            temp_face3[0]	= cell_NodeList[c][2];
                            temp_face3[1]	= cell_NodeList[c][3];
                            temp_face3[2]	= cell_NodeList[c][4];
                            
                            temp_face4[0]	= cell_NodeList[c][3];
                            temp_face4[1]	= cell_NodeList[c][0];
                            temp_face4[2]	= cell_NodeList[c][4];
                            
                            
                            if (faceCompare(temp_face1, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face2, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face3, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face4, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    break;
            }
        } // end left-cell
    }
    
    
    
    // Error check
    for (int c = 0; c <= ncm-1; c++)
    {
        int nfaces;
        int nnodes;
        switch (cell_type[c])
        {
            case TRI3:
                nfaces = 3;
                nnodes = 3;
                break;
                
            case QUAD4:
                nfaces = 4;
                nnodes = 4;
                break;
                
            case TETRA4:
                nfaces = 4;
                nnodes = 4;
                break;
                
            case HEXA8:
                nfaces = 6;
                nnodes = 8;
                break;
                
            case PRISM6:
                nfaces = 5;
                nnodes = 6;
                break;
                
            case PYRAMID5:
                nfaces = 5;
                nnodes = 5;
                break;
        }
        
        if (filled_nodes[c] != nnodes)
        {
            error_message << "Cell ID:[" << c << "] has problem during cell mesh construction. Some of nodes are not filled";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::MismatchData());
        }
        
        if (filled_faces[c] != nfaces)
        {
            error_message << "Cell ID:[" << c << "] has problem during cell mesh construction. Some of faces are not filled";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::MismatchData());
        }
        
        
        
        if (Common::VectorCompare::hasSameComponentINT(cell_NodeList[c], nnodes))
        {
            error_message << "Cell ID:[" << c << "] has repeated node on its node list.";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::ExceedLimit());
        }
        
        if (Common::VectorCompare::hasSameComponentINT(cell_FaceList[c], nfaces))
        {
            error_message << "Cell ID:[" << c << "] has repeated face on its face list.";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::ExceedLimit());
        }
        
        
        if(Common::VectorCompare::hasSatisfyRangeINT(cell_NodeList[c], nnodes, 1, nnm))
        {
            error_message << "Cell ID:[" << c << "] has wrong node index in which is out of possible index range.";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::ExceedLimit());
        }
        
        if(Common::VectorCompare::hasSatisfyRangeINT(cell_FaceList[c], nfaces, 1, nfm))
        {
            error_message << "Cell ID:[" << c << "] has wrong face index in which is out of possible index range.";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::ExceedLimit());
        }
        
    }
    
    mesh_file_Fluent.close();
    
    
    
    
    
    // WRITE OP2A GRID DATA
    std::string fileName;
    std::vector<std::string> fileName_temp;
    
    Common::StringOps::split(mesh_file_name_Fluent, '.', fileName_temp);
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
    mesh_file_OP2 << "NFM= " << nfm << std::endl;
    mesh_file_OP2 << "NCM= " << ncm << std::endl;
    mesh_file_OP2 << std::endl;
    
    mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "NODEDATA= " << nnm <<std::endl;
    if (dim == 2)
    {
        double z = 0.0;
        for (int n_ID = 1; n_ID <= nnm; n_ID++)
        {
            n = node_whereis[n_ID];
            
            mesh_file_OP2 << std::setw(8) << node_ID[n]+1 <<
            std::setw(25) << std::scientific << std::setprecision(16) << node_XY[n][0] <<
            std::setw(25) << node_XY[n][1] <<
            std::setw(25) << z << std::endl;
        }
        
    }
    else
    {
        for (int n_ID = 0; n_ID <= nnm-1; n_ID++)
        {
            n = node_whereis[n_ID];
            
            mesh_file_OP2 << std::setw(8) << n_ID <<
            std::setw(25) << std::scientific << std::setprecision(16) << node_XY[n][0] <<
            std::setw(25) << node_XY[n][1] <<
            std::setw(25) << node_XY[n][2] << std::endl;
        }
    }
    mesh_file_OP2 << std::endl;
    
    
    mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "FACEDATA= " << nfm << std::endl;
    for (int f_ID = 1; f_ID <= nfm; f_ID++)
    {
        f = face_whereis[f_ID];
        
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
        
        mesh_file_OP2 << std::setw(8) << f_ID;
        mesh_file_OP2 << std::setw(8) << face_BC[f];
        mesh_file_OP2 << std::setw(8) << face_type[f];
        
        for (int n = 0; n <= nnode-1; n++) mesh_file_OP2 << std::setw(8) << face_node[f][n];
        mesh_file_OP2 << std::setw(8) << face_stencil[f][0];
        mesh_file_OP2 << std::setw(8) << face_stencil[f][1];
        mesh_file_OP2 << std::endl;
    }
    mesh_file_OP2 << std::endl;
    
    
    mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "CELLDATA= " << ncm << std::endl;
    for (int c_ID = 1; c_ID <= ncm; c_ID++)
    {
        c = cell_whereis[c_ID];
        
        int cellBC = cell_BC[c];
        
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
        
        mesh_file_OP2 << std::setw(8) << c_ID;
        mesh_file_OP2 << std::setw(8) << cellBC;
        mesh_file_OP2 << std::setw(8) << cell_type[c];
        for (int n = 0; n <= nnode-1; n++) mesh_file_OP2 << std::setw(8) << cell_NodeList[c][n];
        for (int f = 0; f <= nface-1; f++) mesh_file_OP2 << std::setw(8) << cell_FaceList[c][f];
        mesh_file_OP2 << std::endl;
    }
    mesh_file_OP2 << std::endl;
    
    
    mesh_file_OP2.close();
}





void precessingGridFLUENT(const  std::string& mesh_file_name_Fluent, std::string& out_file_name)
{
    int index;
    int index_s, index_e;
    int temp_dummy;
    int zone;
    
    std::string line;
    
    // 1. Open file to read
    std::ifstream mesh_file_Fluent;
    mesh_file_Fluent.open(mesh_file_name_Fluent.c_str());
    
    if (!mesh_file_Fluent.is_open()) Common::ExceptionError(FromHere(), "Cannot find grid file. Please check your grid file!", Common::ErrorCodes::FileSystem());
    
    
    // 2. 1st READ
    int dim;
    int nnm;
    int nfm;
    int ncm;
    int nbc;
    int maxZone = 0;
    
    
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        switch(index)
        {
            case 2:
                dim = 0;
                sscanf_s(line.c_str(), "(%d %d)", &temp_dummy, &dim);
                break;
                
            case 10:
                index_s = 0;
                index_e = 0;
                sscanf_s(line.c_str(),"(%d (%x %x %x", &temp_dummy, &zone, &index_s, &index_e); // GETTING INDEX AND ZONE
                if (zone == 0) // READ GENERAL INFORMATION OF NODES
                {
                    nnm = index_e - index_s + 1;
                }
                break;
                
            case 12:
                index_s = 0;
                index_e = 0;
                sscanf_s(line.c_str(),"(%d (%x %x %x", &temp_dummy, &zone, &index_s, &index_e); // GETTING INDEX AND ZONE
                if (zone == 0) // READ GENERAL INFORMATION OF CELLS
                {
                    ncm = index_e - index_s + 1;
                }
                break;
                
            case 13:
                index_s = 0;
                index_e = 0;
                sscanf_s(line.c_str(),"(%d (%x %x %x", &temp_dummy, &zone, &index_s, &index_e); // GETTING INDEX AND ZONE
                
                if (zone == 0) // READ GENERAL INFORMATION OF FACES
                {
                    nfm = index_e - index_s + 1;
                }
                else
                {
                    maxZone = fmax(maxZone, zone);
                }
                break;
        }
    }
    
    // 2. Second Read
    std::vector<int>	nfmZone(maxZone+1, 0);
    std::vector<int>    bcZone(maxZone+1, -1);
    
    
    mesh_file_Fluent.clear();
    mesh_file_Fluent.seekg(0, std::ios::beg);
    
    int temp_zone;
    int bc_type;
    int nfm_zone;
    int num_zone = 0;
    
    int numWall       = 0;
    int numInlet      = 0;
    int numAnode      = 0;
    int numCathode    = 0;
    int numDielectric = 0;
    int numFreestream = 0;
    
    
    char zone_type[20];
    
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        switch(index)
        {
            case 13:
                index_s = 0;
                index_e = 0;
                sscanf_s(line.c_str(),"(%d (%x %x %x", &temp_dummy, &zone, &index_s, &index_e); // GETTING INDEX AND ZONE
                
                if (zone != 0)
                {
                    sscanf_s(line.c_str(),"(%d (%x %x %x %d", &temp_dummy, &temp_zone, &index_s, &index_e, &bc_type); // GETTING INDEX AND ZONE
                    nfm_zone 		= index_e - index_s + 1;
                    nfmZone[zone]	= nfm_zone;
                    num_zone++;
                }
                break;
                
                
            case 45:
                sscanf_s(line.c_str(),"(%d (%d %s", &index, &zone, zone_type); // GETTING INDEX AND ZONE
                
                // GENERAL TYPE OF BOUNDARY CONDITIONS
                bool flagFindBCtype = false;
                
                if (strcmp(zone_type,"interior") == 0)
                {
                    bcZone[zone] = 0;
                    flagFindBCtype = true;
                }
                
                if (strcmp(zone_type,"wall") == 0)
                {
                    if (numWall < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_WALL + numWall;
                        flagFindBCtype = true;
                        numWall++;
                    }
                }
                
                if (strcmp(zone_type,"pressure-inlet") == 0)
                {
                    if (numInlet < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_INLET + numInlet;
                        flagFindBCtype = true;
                        numInlet++;
                    }
                }
                
                if (strcmp(zone_type,"pressure-outlet") == 0)
                {
                    bcZone[zone] = BC_OUTLET;
                    flagFindBCtype = true;
                }
                
                if (strcmp(zone_type,"pressure-far-field") == 0)
                {
                    if (numFreestream < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_FREESTREAM + numFreestream;
                        flagFindBCtype = true;
                        numFreestream++;
                    }
                }
                
                if (strcmp(zone_type,"symmetry") == 0)
                {
                    bcZone[zone] = BC_SYMMETRY;
                    flagFindBCtype = true;
                }
                
                if (strcmp(zone_type,"axis") == 0)
                {
                    bcZone[zone] = BC_AXIS;
                    flagFindBCtype = true;
                }
                
                if (strcmp(zone_type,"anode") == 0)
                {
                    if (numAnode < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_ANODE + numAnode;
                        flagFindBCtype = true;
                        numAnode++;
                    }
                }
                
                if (strcmp(zone_type,"cathode") == 0)
                {
                    if (numCathode < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_CATHODE + numCathode;
                        flagFindBCtype = true;
                        numCathode++;
                    }
                }
                
                if (strcmp(zone_type,"dielectric-wall") == 0)
                {
                    if (numDielectric < BC_MAX_DETAIL_SUBCOND)
                    {
                        bcZone[zone] = BC_DIELECTRIC + numDielectric;
                        flagFindBCtype = true;
                        numDielectric++;
                    }
                }
                
                if (strcmp(zone_type,"fluid") == 0)
                {
                    bcZone[zone] = 0;
                    flagFindBCtype = true;
                }
                
                if (flagFindBCtype == false)
                {
                    Common::ExceptionError(FromHere(), "Given BC type in the grid is not supported BC type. Please check BC type in the grid file", Common::ErrorCodes::NotSupportedType());
                }
                break;
        }
    }
    
    
    // 3. Read Node Data (3rd READ)
    std::vector<int>	                node_ID(nnm,-1);
    std::vector<int>	                node_whereis(nnm+1, -1);
    std::vector<std::vector<double> >	node_XY(nnm, std::vector<double>(dim, 0.0));
    mesh_file_Fluent.clear();
    mesh_file_Fluent.seekg(0, std::ios::beg);
    
    
    int type;
    int temp_dim;
    int n = 0;
    double x, y, z;
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        if (index == 10)
        {
            temp_dummy = -1;
            zone       = -1;
            index_s    = -1;
            index_e    = -1;
            type       = -1;
            temp_dim   = -1;
            
            sscanf_s(line.c_str(),"(%d (%x %x %x %d %d)", &temp_dummy, &zone, &index_s, &index_e, &type, &temp_dim); // GETTING INDEX AND ZONE
            if (zone > 0)
            {
                if (temp_dim != dim)
                    Common::ExceptionError(FromHere(), "PROBLEM IN DIMENSION OF NODE DATA. Please the dimension of grid file", Common::ErrorCodes::MismatchData());
                
                if (dim == 2)
                {
                    for (int i = index_s; i <= index_e; i++)
                    {
                        x = 0.0;
                        y = 0.0;
                        
                        getline(mesh_file_Fluent, line);
                        sscanf_s(line.c_str(), "%lf %lf", &x, &y);
                        
                        node_ID[n]    = n;
                        node_XY[n][0] = x;
                        node_XY[n][1] = y;
                        node_whereis[i] = n;
                        n++;
                    }
                }
                else
                {
                    for (int i = index_s; i <= index_e; i++)
                    {
                        x = 0.0;
                        y = 0.0;
                        z = 0.0;
                        
                        getline(mesh_file_Fluent, line);
                        sscanf_s(line.c_str(), "%lf %lf %lf", &x, &y, &z);
                        
                        node_ID[n]    = i;
                        node_XY[n][0] = x;
                        node_XY[n][1] = y;
                        node_XY[n][2] = z;
                        node_whereis[i] = n;
                        n++;
                    }
                }
            }
        }
    }
    
    if (n != nnm) Common::ExceptionError(FromHere(), "PROBLEM IN NODE DATA. Number of read node does not match with header", Common::ErrorCodes::MismatchData());
    // END READ NODE DATA
    
    
    
    // 4. Read Face Data (4th READ)
    std::vector<int>               face_type (nfm, -1);
    std::vector<int>               face_ID(nfm, -1);
    std::vector<int>               face_whereis(nfm+1, -1);
    std::vector<int>               face_BC(nfm, 0);
    std::vector<std::vector<int> > face_stencil(nfm, std::vector<int>(2, -1));
    std::vector<std::vector<int> > face_node(nfm, std::vector<int>(MAX_FACE_PER_NODE, -1));
    
    int f = 0;
    
    mesh_file_Fluent.clear();
    mesh_file_Fluent.seekg(0, std::ios::beg);
    
    
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        if (index == 13)
        {
            temp_dummy	= -1;
            zone 		= -1;
            index_s     = -1;
            index_e     = -1;
            bc_type     = -1;
            type 	    = -1;
            
            sscanf_s(line.c_str(),"(%d (%x %x %x %d %d)", &temp_dummy, &zone, &index_s, &index_e, &bc_type, &type); // GETTING INDEX AND ZONE
            
            if (zone > 0)
            {
                // Assign BC type from data
                bc_type	= bcZone[zone];
                
                switch (type)
                {
                    case 0:
                        for (int i = index_s; i <= index_e; i++)
                        {
                            int temp_faceType = -1;
                            getline(mesh_file_Fluent, line);
                            sscanf_s(line.c_str(), "%d", &temp_faceType);
                            
                            std::vector<int> temp_nodeList (MAX_FACE_PER_NODE, -1);
                            int cl = -1;
                            int cr = -1;
                            
                            switch (temp_faceType)
                            {
                                case 2: // LINE
                                    sscanf_s(line.c_str(), "%d %x %x %x %x", &type, &temp_nodeList[0], &temp_nodeList[1], &cl, &cr);
                                    
                                    face_ID[f]         = i;
                                    face_type[f]       = LINE;
                                    face_node[f][0]    = temp_nodeList[0];
                                    face_node[f][1]    = temp_nodeList[1];
                                    face_stencil[f][0] = cl;
                                    face_stencil[f][1] = cr;
                                    break;
                                    
                                case 3: // Triangle
                                    sscanf_s(line.c_str(), "%d %x %x %x %x %x", &type, &temp_nodeList[0], &temp_nodeList[1], &temp_nodeList[2], &cl, &cr);
                                    
                                    face_ID[f]         = i;
                                    face_type[f]       = TRI3;
                                    face_node[f][0]    = temp_nodeList[0];
                                    face_node[f][1]    = temp_nodeList[1];
                                    face_node[f][2]    = temp_nodeList[2];
                                    face_stencil[f][0] = cl;
                                    face_stencil[f][1] = cr;
                                    break;
                                    
                                case 4: // Quadrilateral
                                    sscanf_s(line.c_str(), "%d %x %x %x %x %x %x", &type, &temp_nodeList[0], &temp_nodeList[1], &temp_nodeList[2], &temp_nodeList[3], &cl, &cr);
                                    
                                    face_ID[f]         = i;
                                    face_type[f]       = QUAD4;
                                    face_node[f][0]    = temp_nodeList[0];
                                    face_node[f][1]    = temp_nodeList[1];
                                    face_node[f][2]    = temp_nodeList[2];
                                    face_node[f][3]    = temp_nodeList[3];
                                    face_stencil[f][0] = cl;
                                    face_stencil[f][1] = cr;
                                    break;
                                    
                                default:
                                    Common::ExceptionError(FromHere(), "It is not supported Face-type. PLEASE CHECK THE FACE TYPE IN A MESH FILE!", Common::ErrorCodes::NotSupportedType());
                                    break;
                            }
                            
                            face_BC[f]      = bc_type;
                            face_whereis[i]	= f;
                            f++;
                        }
                        break;
                        
                    case 2: // LINE
                        for (int i = index_s; i <= index_e; i++)
                        {
                            std::vector<int> temp_nodeList (MAX_FACE_PER_NODE, -1);
                            int cl = -1;
                            int cr = -1;
                            
                            getline(mesh_file_Fluent, line);
                            sscanf_s(line.c_str(), "%x %x %x %x", &temp_nodeList[0], &temp_nodeList[1], &cl, &cr);
                            
                            face_ID[f]         = i;
                            face_type[f]       = LINE;
                            face_node[f][0]    = temp_nodeList[0];
                            face_node[f][1]    = temp_nodeList[1];
                            face_stencil[f][0] = cl;
                            face_stencil[f][1] = cr;
                            
                            face_BC[f]         = bc_type;
                            face_whereis[i]	   = f;
                            f++;
                        }
                        break;
                        
                    case 3: // Triangle
                        for (int i = index_s; i <= index_e; i++)
                        {
                            std::vector<int> temp_nodeList (MAX_FACE_PER_NODE, -1);
                            int cl = -1;
                            int cr = -1;
                            
                            getline(mesh_file_Fluent, line);
                            sscanf_s(line.c_str(), "%x %x %x %x %x", &temp_nodeList[0], &temp_nodeList[1], &temp_nodeList[2], &cl, &cr);
                            
                            face_ID[f]         = i;
                            face_type[f]       = TRI3;
                            face_node[f][0]    = temp_nodeList[0];
                            face_node[f][1]    = temp_nodeList[1];
                            face_node[f][2]    = temp_nodeList[2];
                            face_stencil[f][0] = cl;
                            face_stencil[f][1] = cr;
                            
                            face_BC[f]         = bc_type;
                            face_whereis[i]	   = f;
                            f++;
                        }
                        break;
                        
                    case 4: // Quadrilateral
                        for (int i = index_s; i <= index_e; i++)
                        {
                            std::vector<int> temp_nodeList (MAX_FACE_PER_NODE, -1);
                            int cl = -1;
                            int cr = -1;
                            
                            getline(mesh_file_Fluent, line);
                            sscanf_s(line.c_str(), "%x %x %x %x %x %x", &temp_nodeList[0], &temp_nodeList[1], &temp_nodeList[2], &temp_nodeList[3], &cl, &cr);
                            
                            face_ID[f]         = i;
                            face_type[f]       = QUAD4;
                            face_node[f][0]    = temp_nodeList[0];
                            face_node[f][1]    = temp_nodeList[1];
                            face_node[f][2]    = temp_nodeList[2];
                            face_node[f][3]    = temp_nodeList[3];
                            face_stencil[f][0] = cl;
                            face_stencil[f][1] = cr;
                            
                            face_BC[f]         = bc_type;
                            face_whereis[i]	   = f;
                            f++;
                        }
                        break;
                        
                    default:
                        Common::ExceptionError(FromHere(), "It is not supported Face-type. PLEASE CHECK THE FACE TYPE IN A MESH FILE!", Common::ErrorCodes::NotSupportedType());
                        break;
                }
            }
        }
    }
    
    if (f != nfm) Common::ExceptionError(FromHere(), "PROBLEM IN Face DATA. Number of read face does not match with header", Common::ErrorCodes::MismatchData());
    
    
    // 5. Read Cell Data (5th READ)
    std::vector<int>               cell_type(ncm, -1);
    std::vector<int>               cell_ID(ncm, -1);
    std::vector<int>               cell_BC(ncm, -1);
    std::vector<int>               cell_whereis(ncm+1, -1);
    std::vector<std::vector<int> > cell_NodeList(ncm, std::vector<int>(MAX_CELL_PER_NODE, -1));
    std::vector<std::vector<int> > cell_FaceList(ncm, std::vector<int>(MAX_CELL_PER_FACE, -1));
    
    int c = 0;
    int temp_type;
    mesh_file_Fluent.clear();
    mesh_file_Fluent.seekg(0, std::ios::beg);
    
    
    while (! mesh_file_Fluent.eof())
    {
        index = -1;
        getline(mesh_file_Fluent, line);
        sscanf_s(line.c_str(), "(%d ", &index);
        
        if (index == 12)
        {
            temp_dummy	= -1;
            zone 		= -1;
            index_s		= -1;
            index_e		= -1;
            type 		= -1;
            temp_type	= -1;
            
            sscanf_s(line.c_str(),"(%d (%x %x %x %d", &temp_dummy, &zone, &index_s, &index_e, &type); // GETTING INDEX AND ZONE
            if (zone > 0)
            {
                sscanf_s(line.c_str(),"(%d (%x %x %x %d %d)", &temp_dummy, &zone, &index_e, &index_e, &type, &temp_type); // GETTING INDEX AND ZONE
                
                if (type == 1)
                {
                    if (temp_type != 0)
                    {
                        for (int i = index_s; i <= index_e; i++)
                        {
                            cell_ID[c]      = i;
                            cell_BC[c]      = BC_INTERIOR;
                            cell_type[c]    = cellTypeConverterFluent(temp_type);
                            cell_whereis[i]	= c;
                            c++;
                        }
                    }
                    else
                    {
                        for (int i = index_s; i <= index_e; i++)
                        {
                            mesh_file_Fluent >> temp_type;
                            
                            cell_ID[c]      = i;
                            cell_BC[c]      = BC_INTERIOR;
                            cell_type[c]    = cellTypeConverterFluent(temp_type);
                            cell_whereis[i]	= c;
                            c++;
                        }
                    }
                }
            }
        }
    }
    if (c != ncm) Common::ExceptionError(FromHere(), "PROBLEM IN Cell DATA. Number of read cell does not match with header", Common::ErrorCodes::MismatchData());
    
    
    
    
    
    
    /*
     * Construct cells (Node and Face list)
     */
    int flag;
    std::vector<int> filled_nodes(ncm, 0);
    std::vector<int> filled_faces(ncm, 0);
    
    std::ostringstream error_message;
    
    for (int f = 0; f <= nfm-1; f++)
    {
        int cl	= face_stencil[f][0];
        int cr	= face_stencil[f][1];
        
        // For Right Cell
        if (cr != -1)
        {
            c = cell_whereis[cr];
            
            switch (cell_type[c])
            {
                case TRI3:
                    if (face_type[f] != LINE)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][1];
                        cell_NodeList[c][1]	= face_node[f][0];
                        filled_nodes[c]	= 2;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 2)
                    {
                        if (face_node[f][0] == cell_NodeList[c][0])
                        {
                            cell_NodeList[c][2]	= face_node[f][1];
                            filled_nodes[c]	= 3;
                            
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c]++;
                        }
                        else if (face_node[f][1] == cell_NodeList[c][1])
                        {
                            cell_NodeList[c][2]	= face_node[f][0];
                            filled_nodes[c]	= 3;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c]++;
                        }
                    }
                    else
                    {
                        if (filled_faces[c] == 2)
                        {
                            if (face_node[f][0] == cell_NodeList[c][0] && face_node[f][1] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if (face_node[f][0] == cell_NodeList[c][2] && face_node[f][1] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case QUAD4:
                    if (face_type[f] != LINE)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][1];
                        cell_NodeList[c][1]	= face_node[f][0];
                        filled_nodes[c]	= 2;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 2)
                    {
                        if (face_node[f][0] != cell_NodeList[c][0] &&
                            face_node[f][0] != cell_NodeList[c][1] &&
                            face_node[f][1] != cell_NodeList[c][0] &&
                            face_node[f][1] != cell_NodeList[c][1])
                        {
                            cell_NodeList[c][2]	= face_node[f][1];
                            cell_NodeList[c][3]	= face_node[f][0];
                            filled_nodes[c]	= 4;
                            
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c]++;
                        }
                        
                        if (cell_NodeList[c][0] == face_node[f][0] && cell_NodeList[c][1] != face_node[f][1])
                        {
                            if (cell_FaceList[c][3] == -1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        
                        if (cell_NodeList[c][0] != face_node[f][0] && cell_NodeList[c][1] == face_node[f][1])
                        {
                            if (cell_FaceList[c][1] == -1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    else if (filled_nodes[c] == 4)
                    {
                        if (face_node[f][0] == cell_NodeList[c][0] && face_node[f][1] == cell_NodeList[c][3])
                        {
                            if (cell_FaceList[c][3] == -1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][0] == cell_NodeList[c][2] && face_node[f][1] == cell_NodeList[c][1])
                        {
                            if (cell_FaceList[c][1] == -1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    
                    break;
                    
                case TETRA4:
                    if (face_type[f] != TRI3)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][1];
                        cell_NodeList[c][2]	= face_node[f][2];
                        filled_nodes[c]	= 3;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    if (filled_nodes[c] == 3)
                    {
                        if (face_node[f][0] != cell_NodeList[c][0] &&
                            face_node[f][0] != cell_NodeList[c][1] &&
                            face_node[f][0] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][0];
                            filled_nodes[c]++;
                            
                            if(face_node[f][1] == cell_NodeList[c][0] &&
                               face_node[f][2] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][1] == cell_NodeList[c][1] &&
                                    face_node[f][2] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][1] == cell_NodeList[c][2] &&
                                    face_node[f][2] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][1] != cell_NodeList[c][0] &&
                                 face_node[f][1] != cell_NodeList[c][1] &&
                                 face_node[f][1] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][1];
                            filled_nodes[c]++;
                            
                            if(face_node[f][0] == cell_NodeList[c][0] &&
                               face_node[f][2] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][1] &&
                                    face_node[f][2] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][2] &&
                                    face_node[f][2] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][2] != cell_NodeList[c][0] &&
                                 face_node[f][2] != cell_NodeList[c][1] &&
                                 face_node[f][2] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][2];
                            filled_nodes[c]++;
                            
                            if(face_node[f][0] == cell_NodeList[c][1] &&
                               face_node[f][1] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][2] &&
                                    face_node[f][1] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][0] &&
                                    face_node[f][1] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case HEXA8:
                    if (face_type[f] != QUAD4)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][1];
                        cell_NodeList[c][2]	= face_node[f][2];
                        cell_NodeList[c][3]	= face_node[f][3];
                        filled_nodes[c]	= 4;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 4 || filled_nodes[c] == 6)
                    {
                        if(cell_NodeList[c][0] == face_node[f][0]&&
                           cell_NodeList[c][1] == face_node[f][3])
                        {
                            cell_NodeList[c][4]	= face_node[f][1];
                            cell_NodeList[c][5]	= face_node[f][2];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][1]&&
                                cell_NodeList[c][1] == face_node[f][0])
                        {
                            cell_NodeList[c][4]	= face_node[f][2];
                            cell_NodeList[c][5]	= face_node[f][3];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][2]&&
                                cell_NodeList[c][1] == face_node[f][1])
                        {
                            cell_NodeList[c][4]	= face_node[f][3];
                            cell_NodeList[c][5]	= face_node[f][0];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][3]&&
                                cell_NodeList[c][1] == face_node[f][2])
                        {
                            cell_NodeList[c][4]	= face_node[f][0];
                            cell_NodeList[c][5]	= face_node[f][1];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][0]&&
                                cell_NodeList[c][3] == face_node[f][3])
                        {
                            cell_NodeList[c][6]	= face_node[f][1];
                            cell_NodeList[c][7]	= face_node[f][2];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][1]&&
                                cell_NodeList[c][3] == face_node[f][0])
                        {
                            cell_NodeList[c][6]	= face_node[f][2];
                            cell_NodeList[c][7]	= face_node[f][3];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][2]&&
                                cell_NodeList[c][3] == face_node[f][1])
                        {
                            cell_NodeList[c][6]	= face_node[f][3];
                            cell_NodeList[c][7]	= face_node[f][0];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][3]&&
                                cell_NodeList[c][3] == face_node[f][2])
                        {
                            cell_NodeList[c][6]	= face_node[f][0];
                            cell_NodeList[c][7]	= face_node[f][1];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                    }
                    else
                    {
                        std::vector<int> temp_face2(4);
                        std::vector<int> temp_face4(4);
                        std::vector<int> temp_face5(4);
                        
                        temp_face2[0]	= cell_NodeList[c][1];
                        temp_face2[1]	= cell_NodeList[c][5];
                        temp_face2[2]	= cell_NodeList[c][6];
                        temp_face2[3]	= cell_NodeList[2][2];
                        
                        temp_face4[0]	= cell_NodeList[c][0];
                        temp_face4[1]	= cell_NodeList[c][3];
                        temp_face4[2]	= cell_NodeList[c][7];
                        temp_face4[3]	= cell_NodeList[2][4];
                        
                        temp_face5[0]	= cell_NodeList[c][4];
                        temp_face5[1]	= cell_NodeList[c][7];
                        temp_face5[2]	= cell_NodeList[c][6];
                        temp_face5[3]	= cell_NodeList[2][5];
                        
                        if (faceCompare(temp_face2, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        
                        if (faceCompare(temp_face4, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][4]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        
                        if (faceCompare(temp_face5, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][5]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                    }
                    break;
                    
                case PRISM6:
                    if (face_type[f] == TRI3)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][1];
                            cell_NodeList[c][2]	= face_node[f][2];
                            filled_nodes[c] = 3;
                            
                            cell_FaceList[c][0]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 5)
                        {
                            if (cell_NodeList[c][3] == face_node[f][0] &&
                                cell_NodeList[c][4] == face_node[f][2])
                            {
                                cell_NodeList[c][5]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][1] &&
                                     cell_NodeList[c][4] == face_node[f][0])
                            {
                                cell_NodeList[c][5]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][2] &&
                                     cell_NodeList[c][4] == face_node[f][1])
                            {
                                cell_NodeList[c][5]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else if (face_type[f] == QUAD4)
                    {
                        if (filled_nodes[c] == 3)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][3])
                            {
                                cell_NodeList[c][3]	= face_node[f][1];
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][3]	= face_node[f][2];
                                cell_NodeList[c][4]	= face_node[f][3];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][3]	= face_node[f][3];
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][3] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][3]	= face_node[f][0];
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                        else if (filled_nodes[c] == 6)
                        {
                            std::vector<int> temp_face2(4);
                            std::vector<int> temp_face3(4);
                            
                            temp_face2[0]	= cell_NodeList[c][1];
                            temp_face2[1]	= cell_NodeList[c][4];
                            temp_face2[2]	= cell_NodeList[c][5];
                            temp_face2[3]	= cell_NodeList[2][2];
                            
                            temp_face3[0]	= cell_NodeList[c][0];
                            temp_face3[1]	= cell_NodeList[c][2];
                            temp_face3[2]	= cell_NodeList[c][5];
                            temp_face3[3]	= cell_NodeList[2][3];
                            
                            
                            if (faceCompare(temp_face2, face_node[f], QUAD4) == 1)
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face3, face_node[f], QUAD4) == 1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    break;
                    
                case PYRAMID5:
                    if (face_type[f] == QUAD4)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][1];
                            cell_NodeList[c][2]	= face_node[f][2];
                            cell_NodeList[c][3]	= face_node[f][3];
                            
                            filled_nodes[c] = 4;
                            
                            cell_FaceList[c][0]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 3)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][2]	= face_node[f][2];
                                cell_NodeList[c][3]	= face_node[f][3];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][2]	= face_node[f][3];
                                cell_NodeList[c][3]	= face_node[f][0];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][3])
                            {
                                cell_NodeList[c][2]	= face_node[f][0];
                                cell_NodeList[c][3]	= face_node[f][1];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][3] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][2]	= face_node[f][1];
                                cell_NodeList[c][3]	= face_node[f][2];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else if (face_type[f] == TRI3)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][2];
                            cell_NodeList[c][4]	= face_node[f][1];
                            filled_nodes[c] = 3;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 4)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][0] &&
                                     cell_NodeList[c][2] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][1] &&
                                     cell_NodeList[c][2] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][2] &&
                                     cell_NodeList[c][2] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][0] &&
                                     cell_NodeList[c][3] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][1] &&
                                     cell_NodeList[c][3] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][2] &&
                                     cell_NodeList[c][3] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][0] &&
                                     cell_NodeList[c][0] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][1] &&
                                     cell_NodeList[c][0] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][2] &&
                                     cell_NodeList[c][0] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                        else if (filled_nodes[c] == 5)
                        {
                            std::vector<int> temp_face1(3);
                            std::vector<int> temp_face2(3);
                            std::vector<int> temp_face3(3);
                            std::vector<int> temp_face4(3);
                            
                            temp_face1[0]	= cell_NodeList[c][0];
                            temp_face1[1]	= cell_NodeList[c][4];
                            temp_face1[2]	= cell_NodeList[c][1];
                            
                            temp_face2[0]	= cell_NodeList[c][1];
                            temp_face2[1]	= cell_NodeList[c][4];
                            temp_face2[2]	= cell_NodeList[c][2];
                            
                            temp_face3[0]	= cell_NodeList[c][3];
                            temp_face3[1]	= cell_NodeList[c][2];
                            temp_face3[2]	= cell_NodeList[c][4];
                            
                            temp_face4[0]	= cell_NodeList[c][0];
                            temp_face4[1]	= cell_NodeList[c][3];
                            temp_face4[2]	= cell_NodeList[c][4];
                            
                            
                            if (faceCompare(temp_face1, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face2, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face3, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face4, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    break;
            }
        } // end right-cell
        
        
        
        // For Left Cell
        if (cl != -1)
        {
            c = cell_whereis[cl];
            
            switch (cell_type[c])
            {
                case TRI3:
                    if (face_type[f] != LINE)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][1];
                        filled_nodes[c]	= 2;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 2)
                    {
                        if (face_node[f][0] == cell_NodeList[c][1])
                        {
                            cell_NodeList[c][2]	= face_node[f][1];
                            filled_nodes[c]	= 3;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c]++;
                        }
                        else if (face_node[f][1] == cell_NodeList[c][0])
                        {
                            cell_NodeList[c][2]	= face_node[f][0];
                            filled_nodes[c]	= 3;
                            
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c]++;
                        }
                    }
                    else
                    {
                        if (filled_faces[c] == 2)
                        {
                            if (face_node[f][0] == cell_NodeList[c][1] && face_node[f][1] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if (face_node[f][0] == cell_NodeList[c][2] && face_node[f][1] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case QUAD4:
                    if (face_type[f] != LINE)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][1];
                        filled_nodes[c]	= 2;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 2)
                    {
                        if (face_node[f][0] != cell_NodeList[c][0] &&
                            face_node[f][0] != cell_NodeList[c][1] &&
                            face_node[f][1] != cell_NodeList[c][0] &&
                            face_node[f][1] != cell_NodeList[c][1])
                        {
                            cell_NodeList[c][2]	= face_node[f][0];
                            cell_NodeList[c][3]	= face_node[f][1];
                            filled_nodes[c]	= 4;
                            
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c]++;
                        }
                        
                        if (cell_NodeList[c][0] == face_node[f][1] && cell_NodeList[c][1] != face_node[f][0])
                        {
                            if (cell_FaceList[c][3] == -1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        
                        if (cell_NodeList[c][0] != face_node[f][1] && cell_NodeList[c][1] == face_node[f][0])
                        {
                            if (cell_FaceList[c][1] == -1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    else if (filled_nodes[c] == 4)
                    {
                        if (face_node[f][0] == cell_NodeList[c][3] && face_node[f][1] == cell_NodeList[c][0])
                        {
                            if (cell_FaceList[c][3] == -1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][0] == cell_NodeList[c][1] && face_node[f][1] == cell_NodeList[c][2])
                        {
                            if (cell_FaceList[c][1] == -1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case TETRA4:
                    if (face_type[f] != TRI3)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][2];
                        cell_NodeList[c][2]	= face_node[f][1];
                        filled_nodes[c]	= 3;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    if (filled_nodes[c] == 3)
                    {
                        if (face_node[f][0] != cell_NodeList[c][0] &&
                            face_node[f][0] != cell_NodeList[c][1] &&
                            face_node[f][0] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][0];
                            filled_nodes[c]++;
                            
                            if(face_node[f][1] == cell_NodeList[c][0] &&
                               face_node[f][2] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][1] == cell_NodeList[c][1] &&
                                    face_node[f][2] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][1] == cell_NodeList[c][2] &&
                                    face_node[f][2] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][1] != cell_NodeList[c][0] &&
                                 face_node[f][1] != cell_NodeList[c][1] &&
                                 face_node[f][1] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][1];
                            filled_nodes[c]++;
                            
                            if(face_node[f][0] == cell_NodeList[c][1] &&
                               face_node[f][2] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][2] &&
                                    face_node[f][2] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][0] &&
                                    face_node[f][2] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                        else if (face_node[f][2] != cell_NodeList[c][0] &&
                                 face_node[f][2] != cell_NodeList[c][1] &&
                                 face_node[f][2] != cell_NodeList[c][2])
                        {
                            cell_NodeList[c][3] = face_node[f][2];
                            filled_nodes[c]++;
                            
                            if(face_node[f][0] == cell_NodeList[c][0] &&
                               face_node[f][1] == cell_NodeList[c][1])
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][1] &&
                                    face_node[f][1] == cell_NodeList[c][2])
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c]++;
                            }
                            else if(face_node[f][0] == cell_NodeList[c][2] &&
                                    face_node[f][1] == cell_NodeList[c][0])
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c]++;
                            }
                        }
                    }
                    break;
                    
                case HEXA8:
                    if (face_type[f] != QUAD4)
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    
                    if (filled_nodes[c] == 0)
                    {
                        cell_NodeList[c][0]	= face_node[f][0];
                        cell_NodeList[c][1]	= face_node[f][3];
                        cell_NodeList[c][2]	= face_node[f][2];
                        cell_NodeList[c][3]	= face_node[f][1];
                        filled_nodes[c]	= 4;
                        
                        cell_FaceList[c][0]	= face_ID[f];
                        filled_faces[c] = 1;
                    }
                    else if (filled_nodes[c] == 4 || filled_nodes[c] == 6)
                    {
                        if(cell_NodeList[c][0] == face_node[f][0]&&
                           cell_NodeList[c][1] == face_node[f][1])
                        {
                            cell_NodeList[c][4]	= face_node[f][3];
                            cell_NodeList[c][5]	= face_node[f][2];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][1]&&
                                cell_NodeList[c][1] == face_node[f][2])
                        {
                            cell_NodeList[c][4]	= face_node[f][0];
                            cell_NodeList[c][5]	= face_node[f][3];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][2]&&
                                cell_NodeList[c][1] == face_node[f][3])
                        {
                            cell_NodeList[c][4]	= face_node[f][1];
                            cell_NodeList[c][5]	= face_node[f][0];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][0] == face_node[f][3]&&
                                cell_NodeList[c][1] == face_node[f][0])
                        {
                            cell_NodeList[c][4]	= face_node[f][2];
                            cell_NodeList[c][5]	= face_node[f][1];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][0]&&
                                cell_NodeList[c][3] == face_node[f][1])
                        {
                            cell_NodeList[c][6]	= face_node[f][3];
                            cell_NodeList[c][7]	= face_node[f][2];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][1]&&
                                cell_NodeList[c][3] == face_node[f][2])
                        {
                            cell_NodeList[c][6]	= face_node[f][0];
                            cell_NodeList[c][7]	= face_node[f][3];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][2]&&
                                cell_NodeList[c][3] == face_node[f][3])
                        {
                            cell_NodeList[c][6]	= face_node[f][1];
                            cell_NodeList[c][7]	= face_node[f][0];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        else if(cell_NodeList[c][2] == face_node[f][3]&&
                                cell_NodeList[c][3] == face_node[f][0])
                        {
                            cell_NodeList[c][6]	= face_node[f][2];
                            cell_NodeList[c][7]	= face_node[f][1];
                            filled_nodes[c] += 2;
                            
                            cell_FaceList[c][3]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                    }
                    else
                    {
                        std::vector<int> temp_face2(4);
                        std::vector<int> temp_face4(4);
                        std::vector<int> temp_face5(4);
                        
                        temp_face2[0]	= cell_NodeList[c][1];
                        temp_face2[1]	= cell_NodeList[c][2];
                        temp_face2[2]	= cell_NodeList[c][6];
                        temp_face2[3]	= cell_NodeList[2][5];
                        
                        temp_face4[0]	= cell_NodeList[c][0];
                        temp_face4[1]	= cell_NodeList[c][4];
                        temp_face4[2]	= cell_NodeList[c][7];
                        temp_face4[3]	= cell_NodeList[2][3];
                        
                        temp_face5[0]	= cell_NodeList[c][4];
                        temp_face5[1]	= cell_NodeList[c][5];
                        temp_face5[2]	= cell_NodeList[c][6];
                        temp_face5[3]	= cell_NodeList[2][7];
                        
                        if (faceCompare(temp_face2, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][2]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        
                        if (faceCompare(temp_face4, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][4]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                        
                        if (faceCompare(temp_face5, face_node[f], QUAD4) == 1)
                        {
                            cell_FaceList[c][5]	= face_ID[f];
                            filled_faces[c] += 1;
                        }
                    }
                    break;
                    
                case PRISM6:
                    if (face_type[f] == TRI3)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][2];
                            cell_NodeList[c][2]	= face_node[f][1];
                            filled_nodes[c] = 3;
                            
                            cell_FaceList[c][0]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 5)
                        {
                            if (cell_NodeList[c][3] == face_node[f][0] &&
                                cell_NodeList[c][4] == face_node[f][1])
                            {
                                cell_NodeList[c][5]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][1] &&
                                     cell_NodeList[c][4] == face_node[f][2])
                            {
                                cell_NodeList[c][5]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][2] &&
                                     cell_NodeList[c][4] == face_node[f][0])
                            {
                                cell_NodeList[c][5]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][5]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else if (face_type[f] == QUAD4)
                    {
                        if (filled_nodes[c] == 3)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][3]	= face_node[f][3];
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][3]	= face_node[f][0];
                                cell_NodeList[c][4]	= face_node[f][3];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][3])
                            {
                                cell_NodeList[c][3]	= face_node[f][1];
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][3] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][3]	= face_node[f][2];
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                        else if (filled_nodes[c] == 6)
                        {
                            std::vector<int> temp_face2(4);
                            std::vector<int> temp_face3(4);
                            
                            temp_face2[0]	= cell_NodeList[c][1];
                            temp_face2[1]	= cell_NodeList[c][2];
                            temp_face2[2]	= cell_NodeList[c][5];
                            temp_face2[3]	= cell_NodeList[2][4];
                            
                            temp_face3[0]	= cell_NodeList[c][0];
                            temp_face3[1]	= cell_NodeList[c][3];
                            temp_face3[2]	= cell_NodeList[c][5];
                            temp_face3[3]	= cell_NodeList[2][2];
                            
                            
                            if (faceCompare(temp_face2, face_node[f], QUAD4) == 1)
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face3, face_node[f], QUAD4) == 1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else
                    {
                        error_message << "Cell ID:[" << cr << "] has problem during cell mesh construction. It has impossible face-type";
                        Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::NotSupportedType());
                    }
                    break;
                    
                case PYRAMID5:
                    if (face_type[f] == QUAD4)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][3];
                            cell_NodeList[c][2]	= face_node[f][2];
                            cell_NodeList[c][3]	= face_node[f][1];
                            filled_nodes[c] = 4;
                            
                            cell_FaceList[c][0]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 3)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][3])
                            {
                                cell_NodeList[c][2]	= face_node[f][2];
                                cell_NodeList[c][3]	= face_node[f][1];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][2]	= face_node[f][3];
                                cell_NodeList[c][3]	= face_node[f][2];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][2]	= face_node[f][0];
                                cell_NodeList[c][3]	= face_node[f][3];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][3] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][2]	= face_node[f][1];
                                cell_NodeList[c][3]	= face_node[f][0];
                                filled_nodes[c] += 2;
                                
                                cell_FaceList[c][0]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    else if (face_type[f] == TRI3)
                    {
                        if (filled_nodes[c] == 0)
                        {
                            cell_NodeList[c][0]	= face_node[f][0];
                            cell_NodeList[c][1]	= face_node[f][1];
                            cell_NodeList[c][4]	= face_node[f][2];
                            filled_nodes[c] = 3;
                            
                            cell_FaceList[c][1]	= face_ID[f];
                            filled_faces[c] = 1;
                        }
                        else if (filled_nodes[c] == 4)
                        {
                            if (cell_NodeList[c][0] == face_node[f][0] &&
                                cell_NodeList[c][1] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][1] &&
                                     cell_NodeList[c][1] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][0] == face_node[f][2] &&
                                     cell_NodeList[c][1] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][0] &&
                                     cell_NodeList[c][2] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][1] &&
                                     cell_NodeList[c][2] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][1] == face_node[f][2] &&
                                     cell_NodeList[c][2] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][0] &&
                                     cell_NodeList[c][3] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][1] &&
                                     cell_NodeList[c][3] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][2] == face_node[f][2] &&
                                     cell_NodeList[c][3] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][0] &&
                                     cell_NodeList[c][0] == face_node[f][1])
                            {
                                cell_NodeList[c][4]	= face_node[f][2];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][1] &&
                                     cell_NodeList[c][0] == face_node[f][2])
                            {
                                cell_NodeList[c][4]	= face_node[f][0];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            else if (cell_NodeList[c][3] == face_node[f][2] &&
                                     cell_NodeList[c][0] == face_node[f][0])
                            {
                                cell_NodeList[c][4]	= face_node[f][1];
                                filled_nodes[c] += 1;
                                
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                        else if (filled_nodes[c] == 5)
                        {
                            std::vector<int> temp_face1(3);
                            std::vector<int> temp_face2(3);
                            std::vector<int> temp_face3(3);
                            std::vector<int> temp_face4(3);
                            
                            temp_face1[0]	= cell_NodeList[c][0];
                            temp_face1[1]	= cell_NodeList[c][1];
                            temp_face1[2]	= cell_NodeList[c][4];
                            
                            temp_face2[0]	= cell_NodeList[c][1];
                            temp_face2[1]	= cell_NodeList[c][2];
                            temp_face2[2]	= cell_NodeList[c][4];
                            
                            temp_face3[0]	= cell_NodeList[c][2];
                            temp_face3[1]	= cell_NodeList[c][3];
                            temp_face3[2]	= cell_NodeList[c][4];
                            
                            temp_face4[0]	= cell_NodeList[c][3];
                            temp_face4[1]	= cell_NodeList[c][0];
                            temp_face4[2]	= cell_NodeList[c][4];
                            
                            
                            if (faceCompare(temp_face1, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][1]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face2, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][2]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face3, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][3]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                            
                            if (faceCompare(temp_face4, face_node[f], TRI3) == 1)
                            {
                                cell_FaceList[c][4]	= face_ID[f];
                                filled_faces[c] += 1;
                            }
                        }
                    }
                    break;
            }
        } // end left-cell
    }
    
    
    
    // Error check
    for (int c = 0; c <= ncm-1; c++)
    {
        int nfaces;
        int nnodes;
        switch (cell_type[c])
        {
            case TRI3:
                nfaces = 3;
                nnodes = 3;
                break;
                
            case QUAD4:
                nfaces = 4;
                nnodes = 4;
                break;
                
            case TETRA4:
                nfaces = 4;
                nnodes = 4;
                break;
                
            case HEXA8:
                nfaces = 6;
                nnodes = 8;
                break;
                
            case PRISM6:
                nfaces = 5;
                nnodes = 6;
                break;
                
            case PYRAMID5:
                nfaces = 5;
                nnodes = 5;
                break;
        }
        
        if (filled_nodes[c] != nnodes)
        {
            error_message << "Cell ID:[" << c << "] has problem during cell mesh construction. Some of nodes are not filled";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::MismatchData());
        }
        
        if (filled_faces[c] != nfaces)
        {
            error_message << "Cell ID:[" << c << "] has problem during cell mesh construction. Some of faces are not filled";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::MismatchData());
        }
        
        
        
        if (Common::VectorCompare::hasSameComponentINT(cell_NodeList[c], nnodes))
        {
            error_message << "Cell ID:[" << c << "] has repeated node on its node list.";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::ExceedLimit());
        }
        
        if (Common::VectorCompare::hasSameComponentINT(cell_FaceList[c], nfaces))
        {
            error_message << "Cell ID:[" << c << "] has repeated face on its face list.";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::ExceedLimit());
        }
        
        
        if(Common::VectorCompare::hasSatisfyRangeINT(cell_NodeList[c], nnodes, 1, nnm))
        {
            error_message << "Cell ID:[" << c << "] has wrong node index in which is out of possible index range.";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::ExceedLimit());
        }
        
        if(Common::VectorCompare::hasSatisfyRangeINT(cell_FaceList[c], nfaces, 1, nfm))
        {
            error_message << "Cell ID:[" << c << "] has wrong face index in which is out of possible index range.";
            Common::ExceptionError(FromHere(), error_message.str(), Common::ErrorCodes::ExceedLimit());
        }
        
    }
    
    mesh_file_Fluent.close();
    
    
    
    
    
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
    mesh_file_OP2 << "NFM= " << nfm << std::endl;
    mesh_file_OP2 << "NCM= " << ncm << std::endl;
    mesh_file_OP2 << std::endl;
    
    mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "NODEDATA= " << nnm <<std::endl;
    if (dim == 2)
    {
        double z = 0.0;
        for (int n_ID = 1; n_ID <= nnm; n_ID++)
        {
            n = node_whereis[n_ID];
            
            mesh_file_OP2 << std::setw(8) << node_ID[n]+1 <<
            std::setw(25) << std::scientific << std::setprecision(16) << node_XY[n][0] <<
            std::setw(25) << node_XY[n][1] <<
            std::setw(25) << z << std::endl;
        }
        
    }
    else
    {
        for (int n_ID = 0; n_ID <= nnm-1; n_ID++)
        {
            n = node_whereis[n_ID];
            
            mesh_file_OP2 << std::setw(8) << n_ID <<
            std::setw(25) << std::scientific << std::setprecision(16) << node_XY[n][0] <<
            std::setw(25) << node_XY[n][1] <<
            std::setw(25) << node_XY[n][2] << std::endl;
        }
    }
    mesh_file_OP2 << std::endl;
    
    
    mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "FACEDATA= " << nfm << std::endl;
    for (int f_ID = 1; f_ID <= nfm; f_ID++)
    {
        f = face_whereis[f_ID];
        
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
        
        mesh_file_OP2 << std::setw(8) << f_ID;
        mesh_file_OP2 << std::setw(8) << face_BC[f];
        mesh_file_OP2 << std::setw(8) << face_type[f];
        
        for (int n = 0; n <= nnode-1; n++) mesh_file_OP2 << std::setw(8) << face_node[f][n];
        mesh_file_OP2 << std::setw(8) << face_stencil[f][0];
        mesh_file_OP2 << std::setw(8) << face_stencil[f][1];
        mesh_file_OP2 << std::endl;
    }
    mesh_file_OP2 << std::endl;
    
    
    mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
    mesh_file_OP2 << "CELLDATA= " << ncm << std::endl;
    for (int c_ID = 1; c_ID <= ncm; c_ID++)
    {
        c = cell_whereis[c_ID];
        
        int cellBC = cell_BC[c];
        
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
        
        mesh_file_OP2 << std::setw(8) << c_ID;
        mesh_file_OP2 << std::setw(8) << cellBC;
        mesh_file_OP2 << std::setw(8) << cell_type[c];
        for (int n = 0; n <= nnode-1; n++) mesh_file_OP2 << std::setw(8) << cell_NodeList[c][n];
        for (int f = 0; f <= nface-1; f++) mesh_file_OP2 << std::setw(8) << cell_FaceList[c][f];
        mesh_file_OP2 << std::endl;
    }
    mesh_file_OP2 << std::endl;
    
    
    mesh_file_OP2.close();
}
