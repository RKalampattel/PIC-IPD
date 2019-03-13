//! \file
//! \brief Implementation of ParticleList class 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#include "ParticleList.h"

// Default constructor
ParticleList::ParticleList()
{
}


// Constructor
ParticleList::ParticleList(Parameters *parametersList, PICmesh *mesh, int patchID)
{
	this->patchID = patchID;
	parametersList->logMessages("Creating particles vector in patch " + std::to_string(patchID), __FILENAME__, __LINE__, 1);
	
	// TODO: In some case, e.g. if there is an input source along a boundary, it
	// may make sense to start with zero particles in each cell - add an extra
	// parameter to see if this is the case or not, e.g. sourceTerm->[true,false]

	// If 0 < numCellsWithParticles <= numCells, seed particles in a few cells, 
	// else seed particles in every cell
	if (parametersList->numCellsWithParticles < 1 || 
		parametersList->numCellsWithParticles > mesh->numCells)
	{
		parametersList->logBrief("Value of numCellsWithParticles has been changed to " + std::to_string(mesh->numCells), 2);
		parametersList->numCellsWithParticles = mesh->numCells;
	}

	for (int i = 0; i < parametersList->numCellsWithParticles; i++)
	{
		if (parametersList->particleDistribution == "uniform")
		{
			// Check if initialParticlesPerCell is a square number
			if (sqrt(parametersList->initialParticlesPerCell) != round(sqrt(parametersList->initialParticlesPerCell)))
			{
				parametersList->logBrief("Value of initialParticlesPerCell has been changed to 1", 2);
				parametersList->initialParticlesPerCell = 1;
			}
		}
		
		for (int j = 0; j < parametersList->initialParticlesPerCell; j++)
		{
			numParticles++;

			Particle particle(parametersList, mesh, patchID, i + 1, numParticles, j);
			listOfParticles.push_back(particle);
			addToPlotVector(&particle);
			
			mesh->addParticleToCell(particle.cellID, particle.particleID, particle.speciesType);
		}
	}

	maxParticleID = numParticles;

	parametersList->logMessages("Generated " + std::to_string(numParticles) +
		" particles in " + std::to_string(parametersList->numCellsWithParticles) + 
		" cells", __FILENAME__, __LINE__, 1);
}


// Destructor
ParticleList::~ParticleList()
{
}


// Add particle to plotVector
void ParticleList::addToPlotVector(Particle *particle)
{
	plotVector.push_back(particle->position);
	plotVector.back().pop_back();	// Remove third position component
	plotVector.back().push_back(particle->velocity[0]);
	plotVector.back().push_back(particle->velocity[1]);
	plotVector.back().push_back(particle->cellID);
	plotVector.back().push_back(particle->particleID);
	plotVector.back().push_back(particle->speciesType);
}


// Update state of plotVector
void ParticleList::updatePlotVector(Particle *particle)
{
	// TODO: Resizing vectors is not a particularly efficient operation, consider 
	// some other means of storing data for plotting in future

	for (int i = 0; i < plotVector.size(); i++)
	{
		if (plotVector[i][5] == static_cast<double>(particle->particleID))
		{
			plotVector.insert(plotVector.begin() + i, particle->position);
			plotVector[i].pop_back();
			plotVector[i].push_back(particle->velocity[0]);
			plotVector[i].push_back(particle->velocity[1]);
			plotVector[i].push_back(particle->cellID);
			plotVector[i].push_back(particle->particleID);
			plotVector[i].push_back(particle->speciesType);
			plotVector.erase(plotVector.begin() + i + 1);
		}
	}
}


// Remove particle from plotVector
void ParticleList::removeFromPlotVector(int particleID)
{
	for (int i = 0; i < plotVector.size(); i++)
	{
		if (plotVector[i][5] == static_cast<double>(particleID))
		{
			plotVector.erase(plotVector.begin() + i);
			break;
		}
	}
}


// Clear fields member of listOfParticles
void ParticleList::clearFields()
{
	for (Particle& particle : listOfParticles)
	{
		particle.EMfield = { 0.0,0.0,0.0,0.0,0.0,0.0 };
	}
}


// Add particles to a cell
void ParticleList::addParticlesToCell(Parameters *parametersList, PICmesh *mesh, int cellID, int numParticlesToAdd, std::string type)
{
	for (int i = 0; i < numParticlesToAdd; i++)
	{
		numParticles++;
		maxParticleID++;

		Particle particle(parametersList, mesh, patchID, cellID, maxParticleID, type);
		listOfParticles.push_back(particle);
		referenceVector.push_back(&listOfParticles.back());
		addToPlotVector(&particle);

		mesh->addParticleToCell(particle.cellID, particle.particleID, particle.speciesType);
	}
}


// Remove single particle from simulation
void ParticleList::removeParticleFromSim(PICmesh * mesh, int particleID)
{
	std::list<Particle>::iterator particle;
	for (particle = listOfParticles.begin(); particle != listOfParticles.end(); particle++)
	{
		if (particle->particleID == particleID)
		{
			mesh->removeParticleFromCell(particle->cellID, particle->particleID, particle->speciesType);
			listOfParticles.erase(particle);
			break;
			// TODO: Update referenceVector when particles are removed.
		}
	}

	removeFromPlotVector(particleID);
	numParticles--;
}


// Remove particles from a cell
void ParticleList::removeParticlesFromCell(PICmesh * mesh, int cellID, int numParticlesToRemove)
{
	// Check that there are actually enough particles to remove
	if (mesh->cellsVector.cells[cellID - 1].particlesInCell.size() >= numParticlesToRemove)
	{
		for (int i = 0; i < numParticlesToRemove; i++)
		{
			// TODO: Could randomise this so rather than removing the first 'i' 
			// particles in the cell, it removes any number 'i' particles. 
			removeParticleFromSim(mesh, mesh->cellsVector.cells[cellID - 1].particlesInCell[i]);
		}
	}
}


//!< Calculate kinetic energy
double ParticleList::calculateEK()
{
	double EK = 0.0;
	for (Particle& particle : listOfParticles)
	{
		// Kinetic energy is calculated by using velocities from t-0.5*dt 
		// and t-1.5*dt (velocity and oldVelocity), in order to get a result time 
		// centred at t-1*dt (still behind). Need abs to remove noise.
		// TODO: Does this need to include all three velocity components?
		EK += 0.5 * particle.mass *	(abs(particle.velocity[0] *
			particle.oldVelocity[0]) + abs(particle.velocity[1] *
			particle.oldVelocity[1]) + abs(particle.velocity[2] *
			particle.oldVelocity[2]));
	}
	return EK;
}
