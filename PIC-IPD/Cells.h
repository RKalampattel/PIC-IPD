//! \file
//! \brief Definition of Cells class 
//! \author Rahul Kalampattel
//! \date Last updated February 2019

#pragma once

#include "GRID\cell.hpp"

//! \class Cells
//! \brief Contains cell properties, derived from CellBase
class Cells : public CellBase
{
public:
	// Data members
	double left = -1.0;					//!< Left cell boundary
	double right = -1.0;				//!< Right cell boundary
	double top = -1.0;					//!< Top (up) cell boundary
	double bottom = -1.0;				//!< Bottom (down) cell boundary
	double totalWeighting = 0.0;		//!< Sum of weights of all resident particles
	int leftCellID = -1;				//!< ID of adjacent left cell
	int rightCellID = -1;				//!< ID of adjacent right cell
	int topCellID = -1;					//!< ID of adjacent top (up) cell
	int bottomCellID = -1;				//!< ID of adjacent bottom (down) cell
	int periodicX1CellID = -1;			//!< ID of periodic cell, x/z direction (not valid for internal cells)
	int	periodicX2CellID = -1;			//!< ID of periodic cell, y/r direction (not valid for internal cells and axisymmetric cases)
	int numNeutrals = 0;				//!< Number of neutral particles in cell
	int numIons = 0;					//!< Number of ions in cell
	std::string boundaryType;			//!< Position on boundary (internal if FALSE)
	std::string firstNodePosition;		//!< Position of first node
	std::vector<int> particlesInCell;	//!< List of particles in the cell


	// Constructor/destructor
	Cells();							//!< Default constructor
	Cells(CellBase baseCell);			//!< Constructor
	~Cells();							//!< Destructor


	// Methods

};