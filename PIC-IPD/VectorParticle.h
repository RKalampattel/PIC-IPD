//! \file
//! \brief Definition of VectorParticle class 
//! \author Rahul Kalampattel
//! \date Last updated March 2018

#pragma once

#include "Particle.h"

//! \class VectorParticle
//! \brief Create and manage a vector of Particle objects
class VectorParticle
{
private:
	// Data members
	int maxParticleID;								//!< Largest particle ID

	
	// Methods
	void addToPlotVector(Particle *particle);		//!< Add particle to plotVector
	void removeFromPlotVector(int particleID);		//!< Remove particle from plotVector

public:
	// Data members
	std::vector<Particle> particleVector;			//!< Vector of Particle objects
	int numParticles = 0;							//!< Size of particleVector								
	int patchID;									//!< Patch ID
	vector2D plotVector;							//!< Store particle position, velocity, cell ID and particle ID for plotting


	// Constructor/destructor
	VectorParticle();								//!< Default constructor
	VectorParticle(Parameters *parametersList,
		Mesh *mesh, int patchID);					//!< Constructor
	~VectorParticle();								//!< Destructor


	// Methods
	void updatePlotVector(Particle *particle);		//!< Update state of plotVector
	void clearFields();								//!< Clear fields and lorentz members of particleVector
	void addParticleToSim(Parameters * parametersList, 
		Mesh * mesh, int cellID, std::string type); //!< Add particle to simulation
	void removeParticleFromSim(int particleID);		//!< Remove particle from simulation
	double calculateEK();							//!< Calculate kinetic energy
};