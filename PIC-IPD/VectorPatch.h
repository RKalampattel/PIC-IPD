//! \file
//! \brief Definition of VectorPatch class 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#pragma once

#include "Patch.h"

//! \class VectorPatch
//! \brief Create and manage a vector of Patch objects
class VectorPatch
{
public:
	// Data members
	int numErrors = 0;							//!< Sum of errors in Patch objects
	std::vector<Patch> patchesVector;			//!< Vector of Patch objects


	// Constructor/destructor
	VectorPatch();								//!< Default constructor
	VectorPatch(Parameters *parametersList);	//!< Constructor
	~VectorPatch();								//!< Destructor
	

	// Methods
	void startPIC();							//!< Start the PIC loop within a VectorPatch object
	
	// TODO: Initial mesh has already been created by Parameters in main.cpp.
	// Need to divide this mesh in sections (numberOfPatches) connected through 
	// a space filling/Hilbert curve, and assign each to a different MPI process

	// TODO: Additional methods to allow data to be shared between patches at
	// given intervals (e.g. particles that have exited one patch and should be
	// generated in an adjacent patch, smoothing out boundaries, etc.)

	// TODO: Routines for load balancing between patches, easiest way is probably 
	// to look at the particle density per patch (sum of all cells), and equalise
	// by re-dividing the mesh along the Hilbert curve. Also need to consider
	// trade-off between computational expense of the exercise and potential benefits
};
