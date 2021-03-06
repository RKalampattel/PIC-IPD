//! \file
//! \brief Definition of Patch class 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#pragma once

#include <iostream>

#include "Mesh.h"
#include "Parameters.h"
#include "ParticleList.h"

//! \class Patch
//! \brief Handles all aspects of an instance of the simulation
class Patch
{
private:
	// Data members
	double time;										//!< Local simulation time
	Parameters parametersList;							//!< Copy of parameters list
	PICmesh mesh;										//!< Details of mesh
	Mesh FDTDmesh;										//!< Details of FDTD mesh
	ParticleList listOfParticles;						//!< List of resident particles
	bool FDTDgenerated = false;							//!< Set to true once FDTD mesh is generated

	// Intermediate values used in Projector, Interpolator and FDTD
	int cellID;
	int nodeID_0;
	int nodeID_1;
	int nodeID_2;
	int nodeID_3;
	double left;
	double right;
	double top;
	double bottom;
	double x1;
	double x2;
	double v1;
	double v2;
	double charge;
	std::string firstNodePosition;


	// Methods
	void generateParticleOutput(vector2D data, 
		int numParticles, double time);					//!< Generate Tecplot output for particles
	void generateNodeOutput(double time);				//!< Generate Tecplot output for nodes
	void generateGlobalOutput(double EK, double EP, 
		double time);									//!< Generate Tecplot output for global parameters

	void getIntermediateValues(Particle& particle);		//!< Update intermediate values used in Projector and Interpolator 
	void getIntermediateValues(int i);					//!< Update intermediate values used in FDTD

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