//! \file
//! \brief Implementation of Particle class 
//! \author Rahul Kalampattel
//! \date Last updated April 2018

#include "Particle.h"

// Default constructor
Particle::Particle()
{
}


// Initial constructor
Particle::Particle(Parameters *parametersList, Mesh *mesh, int patchID, int cellID, int particleID, int index)
{
	this->particleID = particleID;
	this->cellID = cellID;

	if (parametersList->simulationType == "electron")
	{
		this->basic.q = ELECTRON_CHARGE;
		this->basic.m = ELECTRON_MASS_kg;
	}
	else
	{
		if (parametersList->propellant == "xenon")
		{
			this->basic.q = 0.0;
			this->basic.m = XENON_MASS_kg;
		}
	}

	// Initialise random number generator, distribution in range [0, 1000000]
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 1000000);

	if (parametersList->particleDistribution == "precise")
	{
		// Place particle in cell at location (xInitial, yInitial)
		position.push_back(mesh->cellsVector.cells[cellID - 1].left * (1 - parametersList->initialPosition[0]) +
			mesh->cellsVector.cells[cellID - 1].right * parametersList->initialPosition[0]);		// Cartesian x/cylindrical z
		position.push_back(mesh->cellsVector.cells[cellID - 1].top * parametersList->initialPosition[1] +
			mesh->cellsVector.cells[cellID - 1].bottom * (1 - parametersList->initialPosition[1]));	// Cartesian y/cylindrical r
		position.push_back(0.0);																	// Cartesian z/cylindrical theta
	}
	else if (parametersList->particleDistribution == "random")
	{
		// Place particle at a random location in the cell
		double random1 = dist(rng) / (double)1000000;
		double random2 = dist(rng) / (double)1000000;
		position.push_back(mesh->cellsVector.cells[cellID - 1].left * (1 - random1) +
			mesh->cellsVector.cells[cellID - 1].right * random1);			// Cartesian x/cylindrical z
		position.push_back(mesh->cellsVector.cells[cellID - 1].top * random2 +
			mesh->cellsVector.cells[cellID - 1].bottom * (1 - random2));	// Cartesian y/cylindrical r
		position.push_back(0.0);											// Cartesian z/cylindrical theta
	}
	else if (parametersList->particleDistribution == "uniform")
	{
		// Distribute particles uniformly in cell
		double xratio = (0.5 + static_cast<double>(index % 
			static_cast<int>(sqrt(parametersList->particlesPerCell)))) / 
			sqrt(parametersList->particlesPerCell);
		double yratio = (0.5 + static_cast<double>(floor(index /
			sqrt(parametersList->particlesPerCell)))) / sqrt(parametersList->particlesPerCell);

		position.push_back(mesh->cellsVector.cells[cellID - 1].left * (1 - xratio) +
			mesh->cellsVector.cells[cellID - 1].right * xratio);			// Cartesian x/cylindrical z
		position.push_back(mesh->cellsVector.cells[cellID - 1].top * yratio +
			mesh->cellsVector.cells[cellID - 1].bottom * (1 - yratio));		// Cartesian y/cylindrical r
		position.push_back(0.0);											// Cartesian z/cylindrical theta
	}
	// TODO: Method for distributing particles when simulation is axisymmetric,
	// need to ensure same number of particles per unit cell volume, i.e.
	// cells closer to the axis will have different numbers of particles
	// compared to cells at a distance

	// TODO: Select velocities from a Maxwellian distribution
	// Initial particle velocity
	velocity.push_back(parametersList->initialVelocity[0]);	// u
	velocity.push_back(parametersList->initialVelocity[1]);	// v
	velocity.push_back(0.0);								// w

	// Extra setup for the two-stream instability problem
	if (parametersList->twoStream)
	{
		this->basic.type = 1;
		if ((dist(rng) / (double)1000000 - 0.5) >= 0.0)
		{
			this->basic.type = -1;
			this->velocity[0] *= -1.0;
			this->velocity[1] *= -1.0;
		}
	}
}


// Single particle constructor
Particle::Particle(Parameters *parametersList, Mesh *mesh, int patchID, int cellID, int particleID, std::string type)
{
	this->particleID = particleID;
	this->cellID = cellID;

	if (type == "electron" && (parametersList->simulationType == "full" || parametersList->simulationType == "electron"))
	{
		this->basic.q = ELECTRON_CHARGE;
		this->basic.m = ELECTRON_MASS_kg;
		this->basic.type = -1;
	}
	else if (type == "ion" && (parametersList->simulationType == "full" || parametersList->simulationType == "partial"))
	{
		if (parametersList->propellant == "xenon")
		{
			this->basic.q = -ELECTRON_CHARGE;
			this->basic.m = XENON_MASS_kg - ELECTRON_MASS_kg;
		}
		this->basic.type = 1;
	}
	else if (type == "neutral" && (parametersList->simulationType == "full" || parametersList->simulationType == "partial"))
	{
		if (parametersList->propellant == "xenon")
		{
			this->basic.q = 0.0;
			this->basic.m = XENON_MASS_kg;
		}
		this->basic.type = 0;
	}

	// Initialise random number generator, distribution in range [0, 1000000]
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 1000000);

	// Place particle at a random location in the cell
	double random1 = dist(rng) / (double)1000000;
	double random2 = dist(rng) / (double)1000000;
	position.push_back(mesh->cellsVector.cells[cellID - 1].left * (1 - random1) +
		mesh->cellsVector.cells[cellID - 1].right * random1);			// Cartesian x/cylindrical z
	position.push_back(mesh->cellsVector.cells[cellID - 1].top * random2 +
		mesh->cellsVector.cells[cellID - 1].bottom * (1 - random2));	// Cartesian y/cylindrical r
	position.push_back(0.0);											// Cartesian z/cylindrical theta

	// TODO: Select velocities from a Maxwellian distribution
	// Initial particle velocity
	velocity.push_back(parametersList->initialVelocity[0]);	// u
	velocity.push_back(parametersList->initialVelocity[1]);	// v
	velocity.push_back(0.0);								// w

	// Extra setup for the two-stream instability problem
	if (parametersList->twoStream)
	{
		this->basic.type = 1;
		if ((dist(rng) / (double)1000000 - 0.5) >= 0.0)
		{
			this->basic.type = -1;
			this->velocity[0] *= -1.0;
			this->velocity[1] *= -1.0;
		}
	}
}


// Destructor
Particle::~Particle()
{
}


//!< Calculate magnitude of velocity vector
double Particle::velocityMagnitude()
{
	return sqrt(this->position[1] * this->position[1] +
		this->position[2] * this->position[2]);
}