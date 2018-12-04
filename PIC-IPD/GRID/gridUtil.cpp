//
//  gridUtil.cpp
//  OP2A
//
//  Created by Kim M.K. on 09/11/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#include <stdio.h>
#include "grid.hpp"
#include "../COMM/error_codes.hpp"
#include "../COMM/error_exception.hpp"


int faceTypeNumNode(int facetype)
{
    int nnode;
    
    switch (facetype)
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
            
        default:
            Common::ExceptionError(FromHere(), "It is not supported face-type", Common::ErrorCodes::NotSupportedType());
            break;
    }
    
    return (nnode);
}

int cellTypeNumNode(int celltype)
{
    int nnode;
    
    switch (celltype)
    {
        case TRI3:
            nnode = 3;
            break;
            
        case QUAD4:
            nnode = 4;
            break;
            
        case TETRA4:
            nnode = 4;
            break;
            
        case HEXA8:
            nnode = 8;
            break;
            
        case PRISM6:
            nnode = 6;
            break;
            
        case PYRAMID5:
            nnode = 5;
            break;
            
        default:
            Common::ExceptionError(FromHere(), "It is not supported cell-type", Common::ErrorCodes::NotSupportedType());
            break;
    }
    
    return (nnode);
}


int cellTypeNumFace(int celltype)
{
    int nface;
    
    switch (celltype)
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
            
        case PYRAMID5:
            nface = 5;
            break;
            
        default:
            Common::ExceptionError(FromHere(), "It is not supported cell-type", Common::ErrorCodes::NotSupportedType());
            break;
    }
    
    return (nface);
}

bool isWallTypeBC(int bc)
{
    bool iswall = false;
    
    if (bc >= BC_WALL && bc < BC_WALL + BC_MAX_DETAIL_SUBCOND)
    {
        iswall = true;
    }
    else if (bc >= BC_ANODE && bc < BC_ANODE + BC_MAX_DETAIL_SUBCOND)
    {
        iswall = true;
    }
    else if (bc >= BC_CATHODE && bc < BC_CATHODE + BC_MAX_DETAIL_SUBCOND)
    {
        iswall = true;
    }
    else if (bc >= BC_DIELECTRIC && bc < BC_DIELECTRIC + BC_MAX_DETAIL_SUBCOND)
    {
        iswall = true;
    }
    /*
    else if (bc >= BC_SYMMETRY && bc < BC_SYMMETRY + BC_MAX_DETAIL_SUBCOND)
    {
        iswall = true;
    }
    else if (bc >= BC_AXIS && bc < BC_AXIS + BC_MAX_DETAIL_SUBCOND)
    {
        iswall = true;
    }
    */
    
    return (iswall);
}


void faceNodeNumberingReverse(std::vector<int>& nodeList, int type)
{
    int intTemp;
    
    switch (type)
    {
        case LINE:
            intTemp = nodeList[0];
            nodeList[0] = nodeList[1];
            nodeList[1] = intTemp;
            break;
            
        case TRI3:
            intTemp = nodeList[1];
            nodeList[1] = nodeList[2];
            nodeList[2] = intTemp;
            break;
            
        case QUAD4:
            intTemp = nodeList[1];
            nodeList[1] = nodeList[3];
            nodeList[3] = intTemp;
            break;
    }
}

int faceCompare(std::vector<int>& F1_list, std::vector<int>& F2_list, int type)
{
    int comp = 0;
    
    
    if (type == LINE)
    {
        if (F1_list[0] == F2_list[0] && F1_list[1] == F2_list[1])	comp = 1;
        if (F1_list[0] == F2_list[1] && F1_list[1] == F2_list[0])	comp = -1;
    }
    else if (type == TRI3)
    {
        if (F1_list[0] == F2_list[0])
        {
            if (F1_list[1] == F2_list[1] && F1_list[2] == F2_list[2])	comp = 1;
            if (F1_list[1] == F2_list[2] && F1_list[2] == F2_list[1])	comp = -1;
        }
        else if (F1_list[0] == F2_list[1])
        {
            if (F1_list[1] == F2_list[2] && F1_list[2] == F2_list[0])	comp = 1;
            if (F1_list[1] == F2_list[0] && F1_list[2] == F2_list[2])	comp = -1;
        }
        else if (F1_list[0] == F2_list[2])
        {
            if (F1_list[1] == F2_list[0] && F1_list[2] == F2_list[1])	comp = 1;
            if (F1_list[1] == F2_list[1] && F1_list[2] == F2_list[0])	comp = -1;
        }
    }
    else if (type == QUAD4)
    {
        if (F1_list[0] == F2_list[0])
        {
            if (F1_list[1] == F2_list[1] && F1_list[2] == F2_list[2] &&  F1_list[3] == F2_list[3])	comp = 1;
            if (F1_list[1] == F2_list[3] && F1_list[2] == F2_list[3] &&  F1_list[3] == F2_list[1])	comp = -1;
        }
        else if (F1_list[0] == F2_list[1])
        {
            if (F1_list[1] == F2_list[2] && F1_list[2] == F2_list[3] &&  F1_list[3] == F2_list[0])	comp = 1;
            if (F1_list[1] == F2_list[0] && F1_list[2] == F2_list[3] &&  F1_list[3] == F2_list[2])	comp = -1;
        }
        else if (F1_list[0] == F2_list[2])
        {
            if (F1_list[1] == F2_list[3] && F1_list[2] == F2_list[0] &&  F1_list[3] == F2_list[1])	comp = 1;
            if (F1_list[1] == F2_list[1] && F1_list[2] == F2_list[0] &&  F1_list[3] == F2_list[3])	comp = -1;
        }
        else if (F1_list[0] == F2_list[3])
        {
            if (F1_list[1] == F2_list[0] && F1_list[2] == F2_list[1] &&  F1_list[3] == F2_list[1])	comp = 1;
            if (F1_list[1] == F2_list[2] && F1_list[2] == F2_list[1] &&  F1_list[3] == F2_list[0])	comp = -1;
        }
    }
    
    
    return (comp);
}



