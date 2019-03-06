//! \file
//! \brief Definition of ParticleList class 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#pragma once

#include "Particle.h"

//! \class ParticleList
//! \brief Create and manage a list of Particle objects
class ParticleList
{
private:
	// Data members
	int maxParticleID;								//!< Largest particle ID

	
	// Methods
	void addToPlotVector(Particle *particle);		//!< Add particle to plotVector
	void removeFromPlotVector(int particleID);		//!< Remove particle from plotVector

public:
	// Data members
	std::vector<Particle *> referenceVector;		//!< Vector to quickly index particles
	std::list<Particle> listOfParticles;			//!< List of Particle objects
	int numParticles = 0;							//!< Size of listOfParticles								
	int patchID;									//!< Patch ID
	vector2D plotVector;							//!< Store particle position, velocity, cell ID and particle ID for plotting


	// Constructor/destructor
	ParticleList();									//!< Default constructor
	ParticleList(Parameters *parametersList,
		PICmesh *mesh, int patchID);				//!< Constructor
	~ParticleList();								//!< Destructor


	// Methods
	void updatePlotVector(Particle *particle);		//!< Update state of plotVector
	void clearFields();								//!< Clear fields and lorentz members of listOfParticles
	void addParticlesToCell(Parameters * parametersList, 
		PICmesh * mesh, int cellID, int numParticlesToAdd, 
		std::string type);							//!< Add particles to a cell
	void removeParticleFromSim(PICmesh * mesh,
		int particleID);							//!< Remove single particle from simulation
	void removeParticlesFromCell(PICmesh * mesh, int cellID,
		int numParticlesToRemove);					//!< Remove particles from a cell
	double calculateEK();							//!< Calculate kinetic energy
};