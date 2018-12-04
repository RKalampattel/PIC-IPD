//! \file
//! \brief Definition of VectorCell class 
//! \author Rahul Kalampattel
//! \date Last updated November 2017

#pragma once

#include "Cells.h"

//! \class VectorCell
//! \brief Create and manage a vector of Cells objects
class VectorCell
{
public:
	// Data members
	std::vector<Cells> cells;					//!< Vector of Cells objects


	// Constructor/destructor
	VectorCell();								//!< Default constructor
	~VectorCell();								//!< Destructor


	// Methods
	void allocate(std::vector<CellBase> cells);	//!< Allocate CellBase elements to cells data member
};