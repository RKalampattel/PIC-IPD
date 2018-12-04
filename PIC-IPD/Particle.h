//! \file
//! \brief Definition of Particle class 
//! \author Rahul Kalampattel
//! \date Last updated April 2018

#pragma once

#include "Parameters.h"
#include "Mesh.h"
#include "CHEM\species.hpp"

// TODO: Does the Particle class need to be derived from the species class, or 
// should it just contain an attribute speciesType? Doing so means each instance 
// of the Particle class is a lot 'lighter' computationally, and doesn't contain
// unneeded information...

//! \class Particle
//! \brief Contains all particle data, derived from species class
class Particle : public species
{
public:
	// Data members
	int particleID;							//!< Particle ID
	int cellID;								//!< Current cell ID
	std::vector<double> position;			//!< Particle position vector
	std::vector<double> velocity;			//!< Particle velocity vector
	std::vector<double> oldVelocity
	{ -1.0, -1.0, -1.0 };					//!< Velocity vector from previous time step
	std::vector<double> EMfield
	{ -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };	//!< Electromagnetic field


	// Constructor/destructor
	Particle();								//!< Default constructor
	Particle(Parameters *parametersList, Mesh *mesh, int patchID, int cellID, 
		int particleID, int index);			//!< Initial constructor
	Particle(Parameters *parametersList, Mesh *mesh, int patchID, int cellID,
		int particleID, std::string type);	// Single particle constructor
	~Particle();							//!< Destructor


	// Methods
	double velocityMagnitude();				//!< Calculate magnitude of velocity vector
};