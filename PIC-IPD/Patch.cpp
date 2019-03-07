//! \file
//! \brief Implementation of Patch class 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

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

	mesh = PICmesh(parametersList, "PIC");
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


// Generate Tecplot output for particles
void Patch::generateParticleOutput(vector2D data, int numParticles, double time)
{
	// Plot style can be T (plot all particles at each time step), TA (animated),
	// NT (plot each particle over all time steps) and NTA (animated)  
	writeSolutionXY_NTA_Tecplot(parametersList.tecplotParticleSolution, data, numParticles, time);
}


// Generate Tecplot output for nodes
void Patch::generateNodeOutput(double time)
{
	writeSolutionNodeTecplot(parametersList.tecplotNodeSolution, mesh, time);
}


// Generate Tecplot output for global parameters
void Patch::generateGlobalOutput(double EK, double EP, double time)
{
	writeSolution_T_Tecplot(parametersList.tecplotGlobalSolution, EK, EP, 
		parametersList.maximumNumberOfIterations / parametersList.plotFrequency, time);
}


// Update intermediate values used in Projector and Interpolator
void Patch::getIntermediateValues(Particle& particle)
{
	cellID = particle.cellID - 1;
	nodeID_0 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[0] - 1;
	nodeID_1 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[1] - 1;
	nodeID_2 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[2] - 1;
	nodeID_3 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[3] - 1;

	left = mesh.cellsVector.cells[cellID].left;
	right = mesh.cellsVector.cells[cellID].right;
	top = mesh.cellsVector.cells[cellID].top;
	bottom = mesh.cellsVector.cells[cellID].bottom;

	x1 = particle.position[0];
	x2 = particle.position[1];

	v1 = particle.velocity[0];
	v2 = particle.velocity[1];

	firstNodePosition = mesh.cellsVector.cells[cellID].firstNodePosition;
	charge = particle.charge;
}


// Update intermediate values used in FDTD
void Patch::getIntermediateValues(int i)
{
	cellID = FDTDmesh.nodesVector.nodes[i].PICcellID - 1;
	nodeID_0 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[0] - 1;
	nodeID_1 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[1] - 1;
	nodeID_2 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[2] - 1;
	nodeID_3 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[3] - 1;

	left = mesh.cellsVector.cells[cellID].left;
	right = mesh.cellsVector.cells[cellID].right;
	top = mesh.cellsVector.cells[cellID].top;
	bottom = mesh.cellsVector.cells[cellID].bottom;

	x1 = FDTDmesh.nodesVector.nodes[i].geometry.X.element(0, 0);
	x2 = FDTDmesh.nodesVector.nodes[i].geometry.X.element(1, 0);

	firstNodePosition = mesh.cellsVector.cells[cellID].firstNodePosition;
}


// Start the PIC loop within a Patch object
void Patch::startPIC()
{
	numErrors = parametersList.numErrors;
	if (numErrors == 0)
	{
		parametersList.logMessages("Starting PIC loop in patch " + std::to_string(patchID), __FILENAME__, __LINE__, 1);

		for (Particle& particle : listOfParticles.listOfParticles)
		{
			listOfParticles.referenceVector.push_back(&particle);
		}

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
				if (listOfParticles.numParticles < (parametersList.maximumParticlesPerCell * mesh.numCells))
				{
					for (int j = 0; j < mesh.numCells; j++)
					{	
						// TODO: Confirm that cells are ordered in the same way as
						// the elements of numParticlesToModify, i.e. that cells[0]
						// has ID of 1, cells[1] has ID of 2, etc.
						if (numParticlesToModify[j] != 0)
						{
							// Calculate total weight of each cell
							for (int k = 0; k < mesh.cellsVector.cells[j].particlesInCell.size(); k++)
							{
								int particleID = mesh.cellsVector.cells[j].particlesInCell[k];

								// At the moment the process for removing particles 
								// from the simulation is inefficient, as each 
								// time we need to iterate through the entire
								// list of particles until we find the ones with 
								// the right ID. A solution is to have a better 
								// way to identify the particles, e.g. rather than 
								// using an int, maybe have a pointer. This is 
								// implemented through listOfParticles.referenceVector

								// Method 1
								for (Particle& particle : listOfParticles.listOfParticles)
								{
									if (particle.particleID == particleID)
									{
										mesh.cellsVector.cells[j].totalWeighting += particle.particleWeight;
									}
								}

								// Method 2
								// mesh.cellsVector.cells[j].totalWeighting += 
								//	listOfParticles.referenceVector[particleID - 1]->particleWeight;
							}

							// TODO: Decide on what type the particles to add will be,
							// currently electron is selected but this will not always
							// be the right option.

							// Second condition is included to prevent adding particles
							// to a cell that was previously empty
							if (numParticlesToModify[j] > 0 && mesh.cellsVector.cells[j].totalWeighting != 0.0)
							{
								listOfParticles.addParticlesToCell(&parametersList, &mesh, j + 1, numParticlesToModify[j], "electron");
							}
							else if (numParticlesToModify[j] < 0)
							{
								listOfParticles.removeParticlesFromCell(&mesh, j + 1, numParticlesToModify[j]);
							}

							double updatedWeight = mesh.cellsVector.cells[j].totalWeighting /
								static_cast<double>(mesh.cellsVector.cells[j].particlesInCell.size());
							mesh.cellsVector.cells[j].totalWeighting = 0.0;

							// Adjust particle weights and recalculate properties

							for (int k = 0; k < mesh.cellsVector.cells[j].particlesInCell.size(); k++)
							{
								int particleID = mesh.cellsVector.cells[j].particlesInCell[k];

								// Method 1
								for (Particle& particle : listOfParticles.listOfParticles)
								{
									if (particle.particleID == particleID)
									{
										particle.reWeightProperties(updatedWeight);
									}
								}

								// Method 2
								// listOfParticles.referenceVector[particleID - 1]->reWeightProperties(updatedWeight);
							}
						}
					}
				}
			}
		}
	}
}