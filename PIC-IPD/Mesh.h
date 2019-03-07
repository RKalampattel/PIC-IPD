//! \file
//! \brief Definition of Mesh class 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#pragma once

#include "VectorCell.h"
#include "VectorFace.h"
#include "VectorNode.h"

class Parameters;	// Forward declaration to resolve circular dependency 

//! \class Mesh
//! \brief Compiled mesh properties extracted from gridinfo and gridgeo
class Mesh
{
public:
	// Data members
	int numCells;							//!< Number of cells
	int numFaces;							//!< Number of faces
	int numNodes;							//!< Number of nodes
	int dimension;							//!< Dimension of mesh (2D)
	int numRows;							//!< Number of rows of cells
	int numColumns;							//!< Number of columns of cells
	double h;								//!< Cell width/length (assume uniform)
	VectorCell cellsVector;					//!< Vector of cells
	VectorFace facesVector;					//!< Vector of faces
	VectorNode nodesVector;					//!< Vector of nodes


	// Constructor/destructor
	Mesh();									//!< Default constructor
	Mesh(Parameters *localParametersList, 
		std::string type);					//!< Constructor
	~Mesh();								//!< Destructor


	// Methods

};


//! \class PICmesh
//! \brief Derived version of mesh class for PIC mesh
class PICmesh : public Mesh
{
public:
	// Data members
	int minimumParticlesPerCell;			//!< Minimum particles per cell (from Parameters)
	int maximumParticlesPerCell;			//!< Maximum particles per cell (from Parameters)


	// Constructor/destructor
	PICmesh();								//!< Default constructor
	PICmesh(Parameters *localParametersList,
		std::string type);					//!< Constructor
	~PICmesh();								//!< Destructor


	// Methods
	void addParticleToCell(int cellID,
		int particleID, int particleType);	//!< Assign particle IDs to a cell
	void removeParticleFromCell(int cellID,
		int particleID, int particleType);  //!< Remove particle IDs from a cell
	std::vector<int> checkParticleDensity();//!< Check particle density per cell
};