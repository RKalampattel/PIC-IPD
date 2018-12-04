//! \file
//! \brief Definition of VectorPatch class 
//! \author Rahul Kalampattel
//! \date Last updated March 2018

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
};

/*//! \class Simulation
//! \brief TODO: Assign mesh to patches and perform load balancing
class Simulation
{
public:
// Data members


// Constructor/destructor
Simulation();							//!< Default constructor
Simulation(Parameters *parametersList);	//!< Constructor
~Simulation();							//!< Destructor


// Methods
// TODO: Divide mesh in sections (numberOfPatches) connected through a space
// filling/Hilbert curve, and assign each to a different MPI process

// TODO: Additional methods to allow data to be shared between patches at
// given intervals (e.g. particles that have exited one patch and should be
// generated in an adjacent patch, smoothing out boundaries, etc.)
};*/