//! \file
//! \brief Implementation of Patch class 
//! \author Rahul Kalampattel
//! \date Last updated February 2019

#include "Patch.h"

// Default constructor
Patch::Patch()
{
}


// Constructor
Patch::Patch(Parameters *parametersList, int patchID)
{
	parametersList->logMessages("Initialising patch " + std::to_string(patchID), __FILENAME__, __LINE__, 1);

	time = 0;
	this->patchID = patchID;
	this->parametersList = *parametersList;

	mesh = Mesh(parametersList, "PIC");
	listOfParticles = ParticleList(parametersList, &mesh, patchID);

	parametersList->logBrief("Initialising Tecplot output files", 1);
	writeMeshTecplot(parametersList->tecplotMesh, mesh);

	generateParticleOutput(listOfParticles.plotVector, listOfParticles.numParticles, time);
	generateNodeOutput(time);
	generateGlobalOutput(0.0, 0.0, time);
}


// Destructor
Patch::~Patch()
{
}


// Generate Tecplot output
void Patch::generateParticleOutput(vector2D data, int numParticles, double time)
{
	// Plot style can be T (plot all particles at each time step), TA (animated),
	// NT (plot each particle over all time steps) and NTA (animated)  
	writeSolutionXY_NTA_Tecplot(parametersList.tecplotParticleSolution, data, numParticles, time);
}

void Patch::generateNodeOutput(double time)
{
	writeSolutionNodeTecplot(parametersList.tecplotNodeSolution, mesh, time);
}

void Patch::generateGlobalOutput(double EK, double EP, double time)
{
	writeSolution_T_Tecplot(parametersList.tecplotGlobalSolution, EK, EP, 
		parametersList.maximumNumberOfIterations / parametersList.plotFrequency, time);
}

// Start the PIC loop within a Patch object
void Patch::startPIC()
{
	numErrors = parametersList.numErrors;
	if (numErrors == 0)
	{
		parametersList.logMessages("Starting PIC loop in patch " + std::to_string(patchID), __FILENAME__, __LINE__, 1);

		for (int i = 0; i < parametersList.maximumNumberOfIterations; i++)
		{
			parametersList.logMessages("Starting iteration " + std::to_string(i + 1),
				__FILENAME__, __LINE__, 1);

			Projector();

			Solver();

			if ((static_cast<int>(time / parametersList.timeStep) + 1) % parametersList.FDTDfrequency == 0)
			{
				if (FDTDgenerated == false)
				{
					parametersList.processMesh("FDTD");
					FDTDmesh = Mesh(&parametersList, "FDTD");
					FDTDpreprocess();
					FDTDgenerated = true;
				}
				FDTD();
			}

			Interpolator();

			Pusher();

			if ((static_cast<int>(time / parametersList.timeStep) + 1) % parametersList.MCCfrequency == 0)
			{
				MCC();
			}

			// TODO: At certain intervals, calculate the Debye length, plasma  
			// frequency, etc. in order to check that initial assumptions and 
			// methods used are still valid, e.g. is spatial grid still fine enough 
			// to resolve Debye length? Stability of leapfrog method and field 
			// solver?? Where necessary, make changes to fix issues

			// TODO: Pseudocode for checking and fixing plasma density
			// 1. Add function to Mesh class that checks the number of particles 
			// in each cell, and returns the lowest number 
			// 2. The difference between this and parametersList.minimumParticlePerCell
			// is the number of new particles that needs to be created per cell
			// 3. Use the addParticleToSim function in ParticleList to add this
			// this many particles to each cell (may need to overload this function)
			// 4. Need to make sure that we don't add excessive number of particles
			// each loop, otherwise we will cause a memory leak, i.e. need to set
			// a limit for max number of particle are well
			// 5. Can alternatively remove particles from cells with too many, 
			// in which case would need to overload removeParticleFromSim to work 
			// based on cellID, i.e. removing particles with ID in particlesInCell

			// TODO: Add checks to see if variables exceed an allowable range 
			// during operation, e.g. speeds greater than the speed of light, etc.

			numErrors = parametersList.numErrors;
			if (numErrors != 0)
			{
				break;
			}

			time += parametersList.timeStep;

			// Generate plots at specified intervals
			if ((static_cast<int>(time / parametersList.timeStep) + 1) % parametersList.plotFrequency == 0)
			{
				double EK = listOfParticles.calculateEK();
				double EP = mesh.nodesVector.calculateEP();

				generateParticleOutput(listOfParticles.plotVector, listOfParticles.numParticles, time);
				generateNodeOutput(time);
				generateGlobalOutput(EK, EP, time);
				parametersList.logBrief("Tecplot output generated", 1);
			}
		}
	}
}