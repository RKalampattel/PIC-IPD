//! \file
//! \brief Definition of Cells class 
//! \author Rahul Kalampattel
//! \date Last updated March 2018

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
	int leftCellID = -1;				//!< ID of adjacent left cell
	int rightCellID = -1;				//!< ID of adjacent right cell
	int topCellID = -1;					//!< ID of adjacent top (up) cell
	int bottomCellID = -1;				//!< ID of adjacent bottom (down) cell
	int periodicX1CellID = -1;			//!< ID of periodic cell, x/z direction (not valid for internal cells)
	int	periodicX2CellID = -1;			//!< ID of periodic cell, y/r direction (not valid for internal cells and axisymmetric cases)
	std::string boundaryType;			//!< Position on boundary (internal if FALSE)
	std::string firstNodePosition;		//!< Position of first node
	std::vector<int> listOfParticles;	//!< List of particles in the cell


	// Constructor/destructor
	Cells();							//!< Default constructor
	Cells(CellBase baseCell);			//!< Constructor
	~Cells();							//!< Destructor


	// Methods

};