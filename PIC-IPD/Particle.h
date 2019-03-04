//! \file
//! \brief Definition of Particle class 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#pragma once

#include "Parameters.h"
#include "Mesh.h"

//! \class Particle
//! \brief Contains all particle data, derived from species class
class Particle
{
public:
	// Data members
	int particleID;							//!< Particle ID
	int cellID;								//!< Current cell ID
	int speciesType;						//!< Species type
	double mass;							//!< Species mass
	double charge;							//!< Species charge
	double particleWeight;					//!< Particle specific weight
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
	void reWeightProperties
		(double updatedWeight);				//!< Update particle properties when weighting changes 
};