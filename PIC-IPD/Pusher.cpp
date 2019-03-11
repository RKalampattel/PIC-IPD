//! \file
//! \brief Implementation of Pusher method 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#include "Patch.h"

// Update particle position and velocity at each time step
void Patch::Pusher()
{
	// TODO: Consider working with normalised variable (e.g. x/h instead of x, 
	// t/timeStep instead of t, v*timeStep/h instead of v, etc.) in order to reduce 
	// number of operations at each stage

	// Leapfrog method
	if (time == 0.0)
	{
		for (Particle& particle : listOfParticles.listOfParticles)
		{
			particle.oldVelocity = particle.velocity;

			particle.velocity[0] -=
				particle.charge * 
				(particle.EMfield[0] +
					particle.EMfield[5] * 
					particle.velocity[1] - 
					particle.EMfield[4] *
					particle.velocity[2]) * 0.5 *
				parametersList.timeStep / particle.mass;
		
			particle.velocity[1] -=
				particle.charge *
				(particle.EMfield[1] +
					particle.EMfield[3] *
					particle.velocity[2] -
					particle.EMfield[5] * 
					particle.oldVelocity[0]) * 0.5 *
				parametersList.timeStep / particle.mass;

			particle.velocity[2] -=
				particle.charge *
				(particle.EMfield[2] +
					particle.EMfield[4] * 
					particle.oldVelocity[0] -
					particle.EMfield[3] * 
					particle.oldVelocity[1]) * 0.5 *
				parametersList.timeStep / particle.mass;
		}
	}

	// TODO: Inject new particles to maintain charge balance with open BCs

	// TODO: At the moment both Dirichlet and Neumann BCs cause reflection, however
	// in waves, Dirichlet causes a sign change while Neumann does not (perfect 
	// reflection) - check whether this is also correct for particle simulation
	
	// Currently available BCs: periodic, open, Dirichlet and Neumann
	for (Particle& particle : listOfParticles.listOfParticles)
	{		
		particle.oldVelocity = particle.velocity;

		// Update velocity using Boris method:
		double vMinus[3];
		double tVector[3], sVector[3];
		for (int j = 0; j < 3; j++)
		{
			// 1. Half acceleration
			vMinus[j] = particle.velocity[j] + 0.5 *
				particle.charge * parametersList.timeStep *
				particle.EMfield[j] / particle.mass;

			// 2. Rotation
			double theta = 2.0 * abs(atan(0.5 * particle.EMfield[j+3] *
				parametersList.timeStep * particle.charge /
				particle.mass)) * 180.0 / PI;

			if (theta > 45.0)
			{
				parametersList.logBrief("Rotation angle has exceeded 45 degrees by " + std::to_string(theta - 45), 3);
				break;
			}

			tVector[j] = particle.charge * 0.5 *
				parametersList.timeStep * particle.EMfield[j+3] /
				particle.mass;
			sVector[j] = 2 * tVector[j] / (1 + tVector[j] * tVector[j]);
		}

		double v1Dashed = vMinus[0] + vMinus[1] * tVector[2] - vMinus[2] * tVector[1];
		double v2Dashed = vMinus[1] - vMinus[0] * tVector[2] + vMinus[2] * tVector[0];
		double v3Dashed = vMinus[2] + vMinus[0] * tVector[1] - vMinus[1] * tVector[0];

		double v1Plus = vMinus[0] + v2Dashed * sVector[2] - v3Dashed * sVector[1];
		double v2Plus = vMinus[1] - v1Dashed * sVector[2] + v3Dashed * sVector[0];
		double v3Plus = vMinus[2] + v1Dashed * sVector[1] - v2Dashed * sVector[0];

		// 3. Half acceleration
		particle.velocity[0] = v1Plus + 0.5 *
			particle.charge * parametersList.timeStep *
			particle.EMfield[0] / particle.mass;

		particle.velocity[1] = v2Plus + 0.5 *
			particle.charge * parametersList.timeStep *
			particle.EMfield[1] / particle.mass;

		particle.velocity[2] = v3Plus + 0.5 *
			particle.charge * parametersList.timeStep *
			particle.EMfield[2] / particle.mass;

		// TODO: Does third velocity component need to be included in Courant 
		// number calculation since only 2 spatial dimensions are being modelled?
		double courantNumber = (particle.velocity[0] +
			particle.velocity[1] + 
			particle.velocity[2]) * parametersList.timeStep /
			mesh.h;

		if (courantNumber > 1.0)
		{
			parametersList.logBrief("Consider adjusting time step, CFL condition is " + std::to_string(courantNumber), 2);
			if (courantNumber > 1.5)
			{
				parametersList.logBrief("Stopping pusher, CFL condition exceeded by " + std::to_string(courantNumber - 1.0), 3);
				break;
			}
		}

		// Update Cartesian x/cylindrical z position
		particle.position[0] += parametersList.timeStep * 
			particle.velocity[0]; 

		double displacementL = particle.position[0] - 
			mesh.cellsVector.cells[particle.cellID - 1].left;
		double displacementR = particle.position[0] -
			mesh.cellsVector.cells[particle.cellID - 1].right;

		if ((displacementL < 0.0 && abs(displacementL) >= mesh.h) || (displacementR > 0.0 && abs(displacementR) >= mesh.h))
		{
			parametersList.logBrief("Particle " + std::to_string(particle.particleID) + " has moved more than one cell length", 3);
			break;
		}

		// Update cell ID in Cartesian x/cylindrical z direction, exiting left
		if (displacementL < 0.0)
		{
			mesh.removeParticleFromCell(particle.cellID,
				particle.particleID, particle.speciesType);

			// Particle remains inside domain
			if (mesh.cellsVector.cells[particle.cellID - 1].leftCellID > 0)
			{	
				particle.cellID =
					mesh.cellsVector.cells[particle.cellID - 1].leftCellID;
			}
			// Particle crosses left boundary of domain
			else
			{
				if (parametersList.leftBCType == "periodic")
				{
					particle.cellID =
						mesh.cellsVector.cells[particle.cellID - 1].periodicX1CellID;

					// Shift Cartesian x/ cylindrical z position
					particle.position[0] = displacementL +
						mesh.cellsVector.cells[particle.cellID - 1].right;
				}
				else if (parametersList.leftBCType == "open")
				{
					listOfParticles.removeParticleFromSim(&mesh, particle.particleID);
					//i -= 1;
					continue;
				}
				else if (parametersList.leftBCType == "dirichlet" ||
						 parametersList.leftBCType == "neumann")
				{
					// Reflect particle from boundary
					particle.position[0] = -displacementL +
						mesh.cellsVector.cells[particle.cellID - 1].left;

					// Reverse x velocity
					particle.velocity[0] *= -1.0;
				}

			}

			mesh.addParticleToCell(particle.cellID,
				particle.particleID, particle.speciesType);
		}

		// Update cell ID in Cartesian x/ cylindrical z direction, exiting right
		else if (displacementR > 0.0)
		{
			mesh.removeParticleFromCell(particle.cellID,
				particle.particleID, particle.speciesType);

			// Particle remains inside domain
			if (mesh.cellsVector.cells[particle.cellID - 1].rightCellID > 0)
			{
				particle.cellID =
					mesh.cellsVector.cells[particle.cellID - 1].rightCellID;
			}
			// Particle crosses right boundary
			else
			{
				if (parametersList.rightBCType == "periodic")
				{
					particle.cellID =
						mesh.cellsVector.cells[particle.cellID - 1].periodicX1CellID;

					// Shift Cartesian x/ cylindrical z position
					particle.position[0] = displacementR +
						mesh.cellsVector.cells[particle.cellID - 1].left;
				}
				else if (parametersList.rightBCType == "open")
				{
					listOfParticles.removeParticleFromSim(&mesh, particle.particleID);
					//i -= 1;
					continue;
				}
				else if (parametersList.rightBCType == "dirichlet" ||
						 parametersList.rightBCType == "neumann")
				{
					// Reflect particle from boundary
					particle.position[0] = -displacementR +
						mesh.cellsVector.cells[particle.cellID - 1].right;

					// Reverse Cartesian x/ cylindrical z velocity
					particle.velocity[0] *= -1.0;
				}
			}

			mesh.addParticleToCell(particle.cellID,
				particle.particleID, particle.speciesType);
		}

		// Update Cartesian y/ cylindrical r position
		particle.position[1] += parametersList.timeStep * 
			particle.velocity[1];

		double displacementB = particle.position[1] -
			mesh.cellsVector.cells[particle.cellID - 1].bottom;
		double displacementT = particle.position[1] -
			mesh.cellsVector.cells[particle.cellID - 1].top;

		if ((displacementB < 0.0 && abs(displacementB) >= mesh.h) || (displacementT > 0.0 && abs(displacementT) >= mesh.h))
		{
			parametersList.logBrief("Particle " + std::to_string(particle.particleID) + " has moved more than one cell length", 3);
			break;
		}

		// Update cell ID in Cartesian y/ cylindrical r direction, exiting bottom
		if (displacementB < 0.0)
		{
			mesh.removeParticleFromCell(particle.cellID,
				particle.particleID, particle.speciesType);
			
			// Particle remains inside domain
			if (mesh.cellsVector.cells[particle.cellID - 1].bottomCellID > 0)
			{
				particle.cellID =
					mesh.cellsVector.cells[particle.cellID - 1].bottomCellID;
			}
			// Particle crosses bottom boundary
			else
			{
				// Periodic case not valid for axisymmetric simulations
				if (parametersList.bottomBCType == "periodic")
				{
					particle.cellID =
						mesh.cellsVector.cells[particle.cellID - 1].periodicX2CellID;

					// Shift Cartesian y/ cylindrical r position
					particle.position[1] = displacementB +
						mesh.cellsVector.cells[particle.cellID - 1].top;
				}
				else if (parametersList.bottomBCType == "open")
				{
					listOfParticles.removeParticleFromSim(&mesh, particle.particleID);
					//i -= 1;
					continue;
				}
				else if (parametersList.bottomBCType == "dirichlet" ||
						 parametersList.bottomBCType == "neumann")
				{
					// Reflect particle from boundary
					particle.position[1] = -displacementB +
						mesh.cellsVector.cells[particle.cellID - 1].bottom;

					// Reverse Cartesian y/ cylindrical r velocity
					particle.velocity[1] *= -1.0;
				}
			}

			mesh.addParticleToCell(particle.cellID,
				particle.particleID, particle.speciesType);
		}

		// Update cell ID in Cartesian y/ cylindrical r direction, exiting top
		else if (displacementT > 0.0)
		{
			mesh.removeParticleFromCell(particle.cellID,
				particle.particleID, particle.speciesType);
			
			// Particle remains inside domain
			if (mesh.cellsVector.cells[particle.cellID - 1].topCellID > 0)
			{
				particle.cellID =
					mesh.cellsVector.cells[particle.cellID - 1].topCellID;
			}
			// Particle crosses top boundary
			else
			{
				// Periodic case not valid for axisymmetric simulations
				if (parametersList.topBCType == "periodic")
				{
					particle.cellID =
						mesh.cellsVector.cells[particle.cellID - 1].periodicX2CellID;

					// Shift Cartesian y/ cylindrical r position
					particle.position[1] = displacementT +
						mesh.cellsVector.cells[particle.cellID - 1].bottom;
				}
				else if (parametersList.topBCType == "open")
				{
					listOfParticles.removeParticleFromSim(&mesh, particle.particleID);
					//i -= 1;
					continue;
				}
				else if (parametersList.topBCType == "dirichlet" ||
						 parametersList.topBCType == "neumann")
				{
					// Reflect particle from boundary
					particle.position[1] = -displacementT +
						mesh.cellsVector.cells[particle.cellID - 1].top;

					// Reverse Cartesian y/ cylindrical r velocity
					particle.velocity[1] *= -1.0;
				}
			}

			mesh.addParticleToCell(particle.cellID,
				particle.particleID, particle.speciesType);
		}

		// Update cylindrical theta position (no need to update Cartesian z)
		if (parametersList.axisymmetric)
		{
			particle.position[2] += parametersList.timeStep *
				particle.velocity[2];

			double newX2 = sqrt(particle.position[1] *
				+particle.position[1] +
				+particle.position[2] *
				+particle.position[2]);

			if (newX2 > (static_cast<double>(mesh.numRows) * mesh.h));
			{
				parametersList.logBrief("Out of plane motion has exceeded domain height", 2);
			}

			double rotation = atan(abs(particle.position[2]) /
				particle.position[1]);

			if ((rotation * 180.0 / PI) > 15.0)
			{
				parametersList.logBrief("Out of plane rotation has exceeded 15 degrees", 2);
			}

			displacementT += newX2 - particle.position[1];
			if (displacementT > 0.0 && abs(displacementT) >= mesh.h)
			{
				parametersList.logBrief("Particle " + std::to_string(particle.particleID) + " has moved more than one cell length", 3);
				break;
			}

			// Update r and theta positions
			particle.position[1] = newX2;
			particle.position[2] = 0.0;

			// Rotate velocities back to z-r plane
			double velocity2 = particle.velocity[1];
			particle.velocity[1] = cos(rotation) *
				particle.velocity[1] - sin(rotation) *
				particle.velocity[2];
			particle.velocity[2] = sin(rotation) *
				velocity2 + cos(rotation) *	particle.velocity[2];

			// Update cell ID in Cartesian y/ cylindrical r direction, exiting top
			if (displacementT > 0.0)
			{
				mesh.removeParticleFromCell(particle.cellID,
					particle.particleID, particle.speciesType);

				// Particle remains inside domain
				if (mesh.cellsVector.cells[particle.cellID - 1].topCellID > 0)
				{
					particle.cellID =
						mesh.cellsVector.cells[particle.cellID - 1].topCellID;
				}
				// Particle crosses top boundary
				else
				{
					// TODO: Doesn't make sense to have periodic top BC for cylindrical case
					if (parametersList.topBCType == "periodic")
					{
						particle.cellID =
							mesh.cellsVector.cells[particle.cellID - 1].periodicX2CellID;

						// Shift Cartesian y/ cylindrical r position
						particle.position[1] = displacementT +
							mesh.cellsVector.cells[particle.cellID - 1].bottom;
					}
					else if (parametersList.topBCType == "open")
					{
						listOfParticles.removeParticleFromSim(&mesh, particle.particleID);
						//i -= 1;
						continue;
					}
					else if (parametersList.topBCType == "dirichlet" ||
						parametersList.topBCType == "neumann")
					{
						// Reflect particle from boundary
						particle.position[1] = -displacementT +
							mesh.cellsVector.cells[particle.cellID - 1].top;

						// Reverse Cartesian y/ cylindrical r velocity
						particle.velocity[1] *= -1.0;
					}
				}

				mesh.addParticleToCell(particle.cellID,
					particle.particleID, particle.speciesType);
			}
		}
		listOfParticles.updatePlotVector(&particle);
	}

	// TODO: Shift v forwards half a time step to sync v and x for plotting

	parametersList.logBrief("Particle pusher exited", 1);
}