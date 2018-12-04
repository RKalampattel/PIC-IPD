//
//  element.hpp
//  OP2A
//
//  Created by Kim M.K. on 12/09/2016.
//  Copyright © 2016 Kim M.K. All rights reserved.
//

#ifndef element_hpp
#define element_hpp

#include <stdio.h>
#include "geometry.hpp"
#include "connection.hpp"
#include "index.hpp"
#include "stencilinfo.hpp"

#define MAX_NUMBER_CHILDER_PER_FACE 4
#define MAX_NUMBER_CHILDER_PER_CELL 8



////////////////////////////////////////////////
// 1. Grid  Element class for geo with data
////////////////////////////////////////////////
template <class GEO, class DATA>
class Elem_ver1
{
    // Part A: Data Section
public:
    GEO*  geom;
    DATA* data;
    
    // Part B: Constructoir / Destructor Section
public:
    Elem_ver1();
    Elem_ver1(GEO* i_geom, DATA* i_data);
    
    ~Elem_ver1();
    
public:
    // Part C: Functions
    void assign_ptr(GEO* i_geom, DATA* i_data);
    void assign(GEO& i_geom, DATA& i_data);
};

//      Part B: Constructoir / Destructor Section
template <class GEO, class DATA>
Elem_ver1<GEO, DATA>::Elem_ver1()
{
    geom = NULL;
    data = NULL;
}

template <class GEO, class DATA>
Elem_ver1<GEO, DATA>::~Elem_ver1()
{
    geom = NULL;
    data = NULL;
}

template <class GEO, class DATA>
Elem_ver1<GEO, DATA>::Elem_ver1(GEO* i_geom, DATA* i_data)
{
    geom = i_geom;
    data = i_data;
}

//      Part C: Functions
template <class GEO, class DATA>
void Elem_ver1<GEO, DATA>::assign_ptr(GEO* i_geom, DATA* i_data)
{
    geom = i_geom;
    data = i_data;
}

template <class GEO, class DATA>
void Elem_ver1<GEO, DATA>::assign(GEO& i_geom, DATA& i_data)
{
    geom = new GEO;
    data = new DATA;
    
    memcpy(geom, &i_geom, sizeof(GEO));
    memcpy(data, &i_data, sizeof(DATA));
}



////////////////////////////////////////////////
// 2. Grid  Element class for geo with data
////////////////////////////////////////////////
template <class GEO, class IND, class DATA>
class Elem_ver2
{
    // Part A: Data Section
public:
    GEO*    geom;
    IND*    inde;
    DATA*   data;
    
    // Part B: Constructoir / Destructor Section
public:
    Elem_ver2();
    Elem_ver2(GEO* i_geom, IND* i_inde, DATA* i_data);
    
    ~Elem_ver2();
    
public:
    // Part C: Functions
    void assign_ptr(GEO* i_geom, IND* i_inde, DATA* i_data);
    void assign(GEO& i_geom, IND& i_inde, DATA& i_data);
};

//      Part B: Constructoir / Destructor Section
template <class GEO, class IND, class DATA>
Elem_ver2<GEO, IND, DATA>::Elem_ver2()
{
    geom = NULL;
    inde = NULL;
    data = NULL;
}

template <class GEO, class IND, class DATA>
Elem_ver2<GEO, IND, DATA>::~Elem_ver2()
{
    geom = NULL;
    inde = NULL;
    data = NULL;
}

template <class GEO, class IND, class DATA>
Elem_ver2<GEO, IND, DATA>::Elem_ver2(GEO* i_geom, IND* i_inde, DATA* i_data)
: geom(i_geom), inde(i_inde), data(i_data)
{

}

//      Part C: Functions
template <class GEO, class IND, class DATA>
void Elem_ver2<GEO, IND, DATA>::assign_ptr(GEO* i_geom, IND* i_inde, DATA* i_data)
{
    if (i_geom != NULL)
    {
        if (geom == NULL) geom = new GEO;
        geom = i_geom;
    }
    else
    {
        geom = NULL;
    }
    
    
    if (i_inde != NULL)
    {
        if (inde == NULL) inde = new IND;
        inde = i_inde;
    }
    else
    {
        inde = NULL;
    }
    
    if (i_data != NULL)
    {
        if (data == NULL) data = new DATA;
        data = i_data;
    }
    else
    {
        data = NULL;
    }
}

template <class GEO, class IND, class DATA>
void Elem_ver2<GEO, IND, DATA>::assign(GEO& i_geom, IND& i_inde, DATA& i_data)
{
    geom = new GEO;
    inde = new IND;
    data = new DATA;
    
    memcpy(geom, &i_geom, sizeof(GEO));
    memcpy(inde, &i_inde, sizeof(IND));
    memcpy(data, &i_data, sizeof(DATA));
}
























/*
template <class GEO, class CONN, class DATA>
class GridElement
{
public:
    GEO geometry;
    CONN connectivity;
    DATA data;
    
public:
    GridElement()
    {
        
    }
    
    ~GridElement()
    {
        
    }
};




// 1. Base for Grid Elements (ANY)
class GridElementBase{
public:
    virtual ~GridElementBase() {};
};


// 2. Class for NODE types (Ancestor: GridElementBase)
//   2.1 Normal type
template <class DATA>
class NodeNormal : public GridElementBase
{
    // Part A: Data Section
public:
    GeometryNode    geometry;
    ConnectionNode  connectivity;
    DATA            data;
    
    // Part B: Constructoir / Destructor Section
public:
    NodeNormal() { };
    ~NodeNormal() { };
};

//   2.1.1 Cartesin grid type
template <class DATA>
class NodeCart : public NodeNormal<DATA>
{
    // Part A: Data Section
public:
    grid_index index;

    // Part B: Constructoir / Destructor Section
public:
    NodeCart() { };
    ~NodeCart() { };
};


// FACE Types
//  A. Normal grid
template <class DATA>
class FaceNormal : public GridElement<GeometryFace, ConnectionFace, DATA>
{
public:
    FaceNormal() { };
    ~FaceNormal() { };
};

// B. Cartesian grid
template <class DATA>
class FaceCart : public FaceNormal<DATA>
{
public:
    grid_index index;
    FaceCart* parent;
    std::vector<FaceCart*> children;
    
public:
    FaceCart()
    : parent(NULL)
    {
        children.reserve(MAX_NUMBER_CHILDER_PER_FACE);
    };
    
    ~FaceCart()
    {
        
    };
};



// CELL Types
// A. Normal Grid
template <class DATA>
class CellNormal : public GridElement<GeometryCell, ConnectionCell, DATA>
{
public:
    CellNormal() { };
    ~CellNormal() { };
};

template <class DATA>
class CellPIC : public GridElement<GeometryCellPIC, ConnectionCell, DATA>
{
public:
    CellPIC() { };
    ~CellPIC() { };
};

template <class DATA>
class CellHybrid : public GridElement<GeometryCellHybrid, ConnectionCell, DATA>
{
public:
    CellHybrid() { };
    ~CellHybrid() { };
};

// B. Cartesian Grid
template <class DATA>
class CellCart : public CellNormal<DATA>
{
public:
    grid_index index;
    CellCart* parent;
    std::vector<CellCart*> children;
    
public:
    CellCart()
    : parent(NULL)
    {
        children.reserve(MAX_NUMBER_CHILDER_PER_CELL);
    };
    
    ~CellCart()
    {
        
    };
};

template <class DATA>
class CellCartPIC : public CellPIC<DATA>
{
public:
    grid_index index;
    CellCartPIC* parent;
    std::vector<CellCartPIC*> children;
    
public:
    CellCartPIC()
    : parent(NULL)
    {
        children.reserve(MAX_NUMBER_CHILDER_PER_CELL);
    };
    
    ~CellCartPIC()
    {
        
    };
};

template <class DATA>
class CellCartHybrid : public CellHybrid<DATA>
{
public:
    grid_index index;
    CellCartHybrid* parent;
    std::vector<CellCartHybrid*> children;
    
public:
    CellCartHybrid()
    : parent(NULL)
    {
        children.reserve(MAX_NUMBER_CHILDER_PER_CELL);
    };
    
    ~CellCartHybrid()
    {
        
    };
};

*/










#endif /* element_hpp */
