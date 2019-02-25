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
				double EK = listOfParticles.calculateEK();
				double EP = mesh.nodesVector.calculateEP();

				generateParticleOutput(listOfParticles.plotVector, listOfParticles.numParticles, time);
				generateNodeOutput(time);
				generateGlobalOutput(EK, EP, time);
				parametersList.logBrief("Tecplot output generated", 1);

				// Check particle density before next time step
				std::vector<int> numParticlesToModify = mesh.checkParticleDensity();
				// Check that we don't already have too many particles in the simulation
				if (listOfParticles.listOfParticles.size() < (parametersList.maximumParticlesPerCell * mesh.numCells))
				{
					for (int j = 0; j < mesh.numCells; j++)
					{
						// TODO: After adding particles to the cell, need to adjust 
						// the weighting of the particles so that the total properties
						// within the cell (e.g. mass and charge) remain the same.
						// 1. Need to sum the total weighting of the particles 
						// in the cell.
						// 2. Add particles as required.
						// 3. Divide the total weighting by the new total number
						// of particles to get the new weighting, then apply this.
						// 4. Also need to do the same in reverse when removing 
						// particles from the cell (increase weighting). 
						// 5. Once weighting has been adjusted, need to recalculate 
						// relevant properties. 
						for (int k = 0; k < mesh.cellsVector.cells[j].particlesInCell.size(); k++)
						{
							int particleID = mesh.cellsVector.cells[j].particlesInCell[k];
							for (int l = 0; l < listOfParticles.numParticles; l++)
							{
								std::list<Particle>::iterator particle;
								for (particle = listOfParticles.listOfParticles.begin(); particle != listOfParticles.listOfParticles.end(); particle++)
								{ 
									if (particle->particleID == particleID)
									{
										mesh.cellsVector.cells[j].totalWeighting += particle->particleWeight;
									}
								}
							}
							mesh.cellsVector.cells[j].totalWeighting /= 
								static_cast<double>(mesh.cellsVector.cells[j].particlesInCell.size());
						}

						// TODO: Confirm that cells are ordered in the same way as
						// the elements of numParticlesToModify, i.e. that cells[0]
						// has ID of 1, cells[1] has ID of 2, etc.
						if (numParticlesToModify[j] > 0)
						{
							listOfParticles.addParticlesToCell(&parametersList, &mesh, j+1, numParticlesToModify[j], "0");
						}
						else if (numParticlesToModify[j] < 0)
						{
							listOfParticles.removeParticlesFromCell(&mesh, j + 1, numParticlesToModify[j]);
						}
					}
				}
			}
		}
	}
}