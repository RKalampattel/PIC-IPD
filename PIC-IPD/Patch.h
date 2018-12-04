//! \file
//! \brief Definition of Patch class 
//! \author Rahul Kalampattel
//! \date Last updated June 2018

#pragma once

#include <iostream>

#include "Mesh.h"
#include "Parameters.h"
#include "VectorParticle.h"

//! \class Patch
//! \brief Handles all aspects of an instance of the simulation
class Patch
{
private:
	// Data members
	double time;										//!< Local simulation time
	Parameters parametersList;							//!< Copy of parameters list
	Mesh mesh;											//!< Details of mesh
	Mesh FDTDmesh;										//!< Details of FDTD mesh
	VectorParticle particlesVector;						//!< Vector of resident particles
	bool FDTDgenerated = false;							//!< Set to true once FDTD mesh is generated


	// Methods
	void generateParticleOutput(vector2D data, 
		int numParticles, double time);					//!< Generate Tecplot output for particles
	void generateNodeOutput(double time);	//!< Generate Tecplot output for nodes
	void generateGlobalOutput(double EK, double EP, 
		double time);									//!< Generate Tecplot output for global parameters

	void Projector();									//!< Projects quantities from particle to mesh locations
	void Solver();										//!< Solves the Poisson equation
	void FDTDpreprocess();								//!< Pre-processing for FDTD mesh 
	void FDTD();										//!< Resolves an external EM field using the FDTD method
	void Interpolator();								//!< Interpolate quantities from mesh to particle locations
	void Pusher();										//!< Update particle position and velocity at each time step
	void MCC();											//!< Handles collisions between particles

public:
	// Data members
	int patchID;										//!< Patch ID
	int numErrors;										//!< Public copy of numErrors from parametersList


	// Constructor/destructor
	Patch();											//!< Default constructor
	Patch(Parameters *parametersList, int patchID);		//!< Constructor
	~Patch();											//!< Destructor


	// Methods
	void startPIC();									//!< Start the PIC loop within a Patch object	
};