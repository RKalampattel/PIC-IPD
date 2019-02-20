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

			// TODO: Maintain particle density - Can remove particles from cells 
			// with too many particles.
			// NB: Equal number of particles are currently added to each cell,
			// could alter this. Also, all particles added are currently neutrals,
			// is this what is required?

			// TODO: Check what is happening to Tecplot particle output after
			// second set of plots are saved (getting NaN results for all values)

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
				// Check particle density
				int numParticlesToAdd = mesh.checkParticleDensity();
				if (numParticlesToAdd > 0 && listOfParticles.listOfParticles.size() < parametersList.maximumNumberOfParticles)
				{
					listOfParticles.addParticlesToSim(&parametersList, &mesh, numParticlesToAdd);
					// TODO: After adding particles to the cell, need to adjust 
					// the weighting of the particles so that the total properties
					// within the cell (e.g. mass and charge) remain the same.
					// To do this, need to first sum the total weighting of the 
					// particles in the cell, then add particles as required, 
					// then divide the total weighting by the new total number
					// of particles to get the new weighting, then set this number.
					// Also need to do the same in reverse when removing particles
					// from the cell (increase weighting). 
				}
				
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