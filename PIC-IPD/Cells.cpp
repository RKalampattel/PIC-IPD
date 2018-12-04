//! \file
//! \brief Implementation of Cells class 
//! \author Rahul Kalampattel
//! \date Last updated February 2018

#include "Cells.h"

// Default constructor
Cells::Cells()
{
}


// Constructor
Cells::Cells(CellBase baseCell)
{
	this->connectivity = baseCell.connectivity;
	this->geometry = baseCell.geometry;
}


// Destructor
Cells::~Cells()
{
}