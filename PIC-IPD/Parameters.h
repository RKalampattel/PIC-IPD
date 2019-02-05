//! \file
//! \brief Definition of Parameters class 
//! \author Rahul Kalampattel
//! \date Last updated February 2019

#pragma once

#include <chrono>
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "omp.h"

#include "GRID/grid.hpp"

#define __FILENAME__ strrchr("\\" __FILE__, '\\') + 1

//! \class Parameters 
//! \brief Handles processing of input parameters and logging
class Parameters
{
private:
	// Data members
	std::vector<std::string> valuesVector;	//!< Raw strings from input file	
	std::string meshFilePIC = "PICMesh";	//!< Name of PIC mesh file
	std::string meshFileFDTD = "FDTDMesh";	//!< Name of FDTD mesh file

	bool useDefaultArgument = false;		//!< Flag to use default values for arguments
	bool fileNotOpened = false;				//!< Check if input file was opened 
	bool firstLog = true;					//!< Check if this is the first log entry
	std::chrono::system_clock::time_point initialTime;	//!< Global simulation time

public:
	// Data members
	int numErrors = 0;						//!< Number of errors detected during simulation
	GridBasicInfo gridinfoPIC;				//!< Basic grid properties, PIC mesh
	GridGeo gridgeoPIC;						//!< Detailed grid info, PIC mesh
	GridBasicInfo gridinfoFDTD;				//!< Basic grid properties, FDTD mesh
	GridGeo gridgeoFDTD;					//!< Detailed grid info, FDTD mesh


	// Global simulation parameters
	double timeStep;						//!< Time step
	int maximumNumberOfIterations;			//!< Maximum number of iterations
	int numberOfPatches;					//!< Number of patches
	int particlesPerCell;					//!< Particles per cell
	int numCellsWithParticles;				//!< Number of cells in which particles are seeded	
	std::string simulationType;				//!< Simulation type (full, partial or electron)
	bool axisymmetric;						//!< True if axisymmetric simulation is required
	bool twoStream;							//!< True is two-stream problem is bring modelled

	// Particle and collision parameters
	std::string particleDistribution;		//!< Particle distribution (random, uniform, precise)
	double specificWeight;					//!< Ratio of superparticle to real particle mass 
	double initialTemperature;				//!< Initial temperature of gas/plasma
	std::vector<double> initialPosition;	//!< Initial particle position (if precise==true)
	std::vector<double> initialVelocity;	//!< Initial particle velocity (if precise==true)
	std::string propellant;					//!< Propellant used in simulation (xenon)
	int MCCfrequency;						//!< Iterations between calls to MCC

	// Field and FDTD parameters
	std::vector<double> Efield;				//!< External electric field
	std::vector<double> Bfield;				//!< External magnetic field
	int FDTDiterations;						//!< Number of iterations in FDTD loop, determines FDTD time step
	int FDTDfrequency;						//!< Iterations between calls to FDTD

	// Mesh and domain parameters
	bool userMesh;							//!< If true, use user defined mesh rather than mesh from file
	double domainLength;					//!< Length of simulation domain
	double domainHeight;					//!< Height of simulation domain;
	double PICspacing;						//!< Grid spacing for PIC mesh
	double FDTDspacing;						//!< Grid spacing for FDTD mesh
	std::string meshFilePath;				//!< Path of mesh file
	double meshScalingParameter;			//!< Mesh scaling parameter

	// Solver and boundary condition parameters
	std::string solverType;					//!< Solver type (GS, FFT)
	int maxSolverIterations;				//!< Maximum number of iterations for solver
	double residualTolerance;				//!< Tolerance for solver residuals
	double SORparameter;					//!< Successive over-relaxation parameter
	std::string leftBCType;					//!< Boundary condition on left edge (periodic, open, dirichlet, neumann)
	double leftBCValue;						//!< Value of left edge boundary condition
	std::string rightBCType;				//!< Boundary condition on right edge (periodic, open, dirichlet, neumann)
	double rightBCValue;					//!< Value of right edge boundary condition
	std::string topBCType;					//!< Boundary condition on top edge (periodic, open, dirichlet, neumann)
	double topBCValue;						//!< Value of top edge boundary condition
	std::string bottomBCType;				//!< Boundary condition on bottom edge (periodic, open, dirichlet, neumann)
	double bottomBCValue;					//!< Value of bottom edge boundary condition

	// Parallelisation parameters
	int numThreads;							//!< Number of OpenMP threads for parallelisation
	
	// Output parameters
	int plotFrequency;						//!< Iterations between plots
	std::string tecplotMesh;				//!< Tecplot output mesh file
	std::string tecplotParticleSolution;	//!< Tecplot particle solution file
	std::string tecplotNodeSolution;		//!< Tecplot node solution file
	std::string tecplotGlobalSolution;		//!< Tecplot global solution file


	// Constructor/destructor
	Parameters();							//!< Default constructor
	Parameters(std::string filename);		//!< Constructor
	~Parameters();							//!< Destructor


	// Methods
	void assignInputs();					//!< Assign values to data members
	void generateMesh(std::string type);	//!< Generate a user-defined mesh
	void processMesh(std::string type);		//!< Post process mesh 
	void logMessages(std::string message, std::string filename, 
		int line, int messageType);			//!< Log messages, warnings and errors
	void logBrief(std::string message, 
		int messageType);					//!< Log brief messages
};