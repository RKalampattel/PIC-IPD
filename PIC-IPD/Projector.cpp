//! \file
//! \brief Implementation of Projector method 
//! \author Rahul Kalampattel
//! \date Last updated February 2019

#include "Patch.h"

// Projects quantities from particle to mesh locations
void Patch::Projector()
{
	// Set charge at all nodes to zero at the start of each step
	mesh.nodesVector.clearChargeAndCurrent();

	double hSquared = mesh.h * mesh.h;

	// Project charge to nodes
	for (Particle& particle : listOfParticles.listOfParticles)
	{
		// TODO: Can change all of the below to references to avoid copying large 
		// amounts of data for each calculation

		int cellID = particle.cellID - 1;
		int nodeID_0 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[0] - 1;
		int nodeID_1 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[1] - 1;
		int nodeID_2 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[2] - 1;
		int nodeID_3 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[3] - 1;

		double left = mesh.cellsVector.cells[cellID].left;
		double right = mesh.cellsVector.cells[cellID].right;
		double top = mesh.cellsVector.cells[cellID].top;
		double bottom = mesh.cellsVector.cells[cellID].bottom;
		
		double x1 = particle.position[0];
		double x2 = particle.position[1];

		std::string firstNodePosition = mesh.cellsVector.cells[cellID].firstNodePosition;
		double charge = particle.basic.q;

		if (firstNodePosition == "TL")
		{
			mesh.nodesVector.nodes[nodeID_0].charge += charge * (right - x1) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_1].charge += charge * (right - x1) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_2].charge += charge * (x1 - left) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_3].charge += charge * (x1 - left) * (x2 - bottom) / hSquared;
		}
		else if (firstNodePosition == "BL")
		{
			mesh.nodesVector.nodes[nodeID_0].charge += charge * (right - x1) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_1].charge += charge * (x1 - left) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_2].charge += charge * (x1 - left) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_3].charge += charge * (right - x1) * (x2 - bottom) / hSquared;

		}
		else if (firstNodePosition == "BR")
		{
			mesh.nodesVector.nodes[nodeID_0].charge += charge * (x1 - left) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_1].charge += charge * (x1 - left) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_2].charge += charge * (right - x1) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_3].charge += charge * (right - x1) * (top - x2) / hSquared;
		}
		else if (firstNodePosition == "TR")
		{
			mesh.nodesVector.nodes[nodeID_0].charge += charge * (x1 - left) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_1].charge += charge * (right - x1) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_2].charge += charge * (right - x1) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_3].charge += charge * (x1 - left) * (top - x2) / hSquared;
		}
	}

	// Calculate charge density (charge / cell volume)
	for (int i = 0; i < mesh.numNodes; i++)
	{
		if (parametersList.simulationType == "electron")
		{
			// Assuming a cold plasma with m_ion/m_electron equals infinity, only 
			// electrons are modelled. In order to maintain a quasi-neutral plasma,
			// we assume fixed ions at the nodes, providing a neutralising background 
			// charge density.
			mesh.nodesVector.nodes[i].charge -= (listOfParticles.numParticles * 
				ELECTRON_CHARGE / mesh.numNodes);
		}

		// For cylindrical case, need to account for changing cell volume
		if (parametersList.axisymmetric == true)
		{
			int bottomNodeID = mesh.nodesVector.nodes[i].bottomNodeID - 1;
			if (bottomNodeID >= 0)
			{
				mesh.nodesVector.nodes[i].rho = mesh.nodesVector.nodes[i].charge /
					std::_Pi * mesh.h * (mesh.nodesVector.nodes[i].geometry.X.element(1, 0) *
						mesh.nodesVector.nodes[i].geometry.X.element(1, 0) -
						mesh.nodesVector.nodes[bottomNodeID].geometry.X.element(1, 0) *
						mesh.nodesVector.nodes[bottomNodeID].geometry.X.element(1, 0));
			}
			else
			{
				// Zero cell volume at axis
				mesh.nodesVector.nodes[i].rho = 0.0;
			}
		}
		else
		{
			// Cartesian case, assume unit cell depth
			if (mesh.nodesVector.nodes[i].boundaryType == "internal")
			{
				mesh.nodesVector.nodes[i].rho = mesh.nodesVector.nodes[i].charge / hSquared;
			}
			else if (mesh.nodesVector.nodes[i].boundaryType == "L" ||
				mesh.nodesVector.nodes[i].boundaryType == "R" || 
				mesh.nodesVector.nodes[i].boundaryType == "T" || 
				mesh.nodesVector.nodes[i].boundaryType == "B")
			{
				mesh.nodesVector.nodes[i].rho = mesh.nodesVector.nodes[i].charge / (0.5 * hSquared);
			}
			else
			{
				mesh.nodesVector.nodes[i].rho = mesh.nodesVector.nodes[i].charge / (0.25 * hSquared);
			}
		}
	}

	double sumT = 0.0;
	for (int i = 0; i < mesh.numNodes; i++)
	{
		sumT += mesh.nodesVector.nodes[i].rho;
	}
	sumT += 0.0;

	// Account for periodic BCs 
	for (int i = 0; i < mesh.numNodes; i++)
	{
		if (parametersList.leftBCType == "periodic")	// Periodic left implies periodic right
		{
			if (mesh.nodesVector.nodes[i].boundaryType == "TL" ||
				mesh.nodesVector.nodes[i].boundaryType == "L" ||
				mesh.nodesVector.nodes[i].boundaryType == "BL")
			{
				mesh.nodesVector.nodes[i].rho = 0.5 * (mesh.nodesVector.nodes[i].rho +
					mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX1NodeID - 1].rho);
				mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX1NodeID - 1].rho =
					mesh.nodesVector.nodes[i].rho;
			}
		}
		if (parametersList.topBCType == "periodic")	// Periodic top implies periodic bottom
		{
			if (mesh.nodesVector.nodes[i].boundaryType == "TL" ||
				mesh.nodesVector.nodes[i].boundaryType == "T" ||
				mesh.nodesVector.nodes[i].boundaryType == "TR")
			{
				mesh.nodesVector.nodes[i].rho = 0.5 * (mesh.nodesVector.nodes[i].rho +
					mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX2NodeID - 1].rho);
				mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX2NodeID - 1].rho =
					mesh.nodesVector.nodes[i].rho;
			}
		}
	}

	// TODO: Current calculation involves velocity, which at present in calculated 
	// at half time-steps, i.e. current is also calculated at half time-steps. 
	// Need to make sure this is ok for use with FDTD. If not, can average between
	// velocity and oldVelocity, like in EK calculation (ParticleList).

	// Project current to nodes
	for (Particle& particle : listOfParticles.listOfParticles)
	{
		// TODO: Can change all of the below to references to avoid copying large 
		// amounts of data for each calculation, or make a template to use same
		// variables as in charge projection?

		int cellID = particle.cellID - 1;
		int nodeID_0 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[0] - 1;
		int nodeID_1 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[1] - 1;
		int nodeID_2 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[2] - 1;
		int nodeID_3 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[3] - 1;

		double left = mesh.cellsVector.cells[cellID].left;
		double right = mesh.cellsVector.cells[cellID].right;
		double top = mesh.cellsVector.cells[cellID].top;
		double bottom = mesh.cellsVector.cells[cellID].bottom;

		double x1 = particle.position[0];
		double x2 = particle.position[1];

		double v1 = particle.velocity[0];
		double v2 = particle.velocity[1];

		std::string firstNodePosition = mesh.cellsVector.cells[cellID].firstNodePosition;
		double charge = particle.basic.q;

		if (firstNodePosition == "BL")
		{
			mesh.nodesVector.nodes[nodeID_0].current[0] += mesh.nodesVector.nodes[nodeID_0].rho * 
				v1 * (right - x1) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_0].current[1] += mesh.nodesVector.nodes[nodeID_0].rho *
				v2 * (right - x1) * (top - x2) / hSquared;

			mesh.nodesVector.nodes[nodeID_1].current[0] += mesh.nodesVector.nodes[nodeID_1].rho * 
				v1 * (x1 - left) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_1].current[1] += mesh.nodesVector.nodes[nodeID_1].rho *
				v2 * (x1 - left) * (top - x2) / hSquared;

			mesh.nodesVector.nodes[nodeID_2].current[0] += mesh.nodesVector.nodes[nodeID_2].rho * 
				v1 * (x1 - left) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_2].current[1] += mesh.nodesVector.nodes[nodeID_2].rho *
				v2 * (x1 - left) * (x2 - bottom) / hSquared;

			mesh.nodesVector.nodes[nodeID_3].current[0] += mesh.nodesVector.nodes[nodeID_3].rho *
				v1 * (right - x1) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_3].current[1] += mesh.nodesVector.nodes[nodeID_3].rho *
				v2 * (right - x1) * (x2 - bottom) / hSquared;
		}
		else if (firstNodePosition == "BR")
		{
			mesh.nodesVector.nodes[nodeID_0].current[0] += mesh.nodesVector.nodes[nodeID_0].rho * 
				v1 * (x1 - left) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_0].current[1] += mesh.nodesVector.nodes[nodeID_0].rho *
				v2 * (x1 - left) * (top - x2) / hSquared;

			mesh.nodesVector.nodes[nodeID_1].current[0] += mesh.nodesVector.nodes[nodeID_1].rho * 
				v1 * (x1 - left) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_1].current[1] += mesh.nodesVector.nodes[nodeID_1].rho *
				v2 * (x1 - left) * (x2 - bottom) / hSquared;

			mesh.nodesVector.nodes[nodeID_2].current[0] += mesh.nodesVector.nodes[nodeID_2].rho * 
				v1 * (right - x1) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_2].current[1] += mesh.nodesVector.nodes[nodeID_2].rho *
				v2 * (right - x1) * (x2 - bottom) / hSquared;

			mesh.nodesVector.nodes[nodeID_3].current[0] += mesh.nodesVector.nodes[nodeID_3].rho * 
				v1 * (right - x1) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_3].current[1] += mesh.nodesVector.nodes[nodeID_3].rho *
				v2 * (right - x1) * (top - x2) / hSquared;
		}
		else if (firstNodePosition == "TR")
		{
			mesh.nodesVector.nodes[nodeID_0].current[0] += mesh.nodesVector.nodes[nodeID_0].rho * 
				v1 * (x1 - left) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_0].current[1] += mesh.nodesVector.nodes[nodeID_0].rho *
				v2 * (x1 - left) * (x2 - bottom) / hSquared;

			mesh.nodesVector.nodes[nodeID_1].current[0] += mesh.nodesVector.nodes[nodeID_1].rho * 
				v1 * (right - x1) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_1].current[1] += mesh.nodesVector.nodes[nodeID_1].rho *
				v2 * (right - x1) * (x2 - bottom) / hSquared;

			mesh.nodesVector.nodes[nodeID_2].current[0] += mesh.nodesVector.nodes[nodeID_2].rho * 
				v1 * (right - x1) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_2].current[1] += mesh.nodesVector.nodes[nodeID_2].rho *
				v2 * (right - x1) * (top - x2) / hSquared;

			mesh.nodesVector.nodes[nodeID_3].current[0] += mesh.nodesVector.nodes[nodeID_3].rho * 
				v1 * (x1 - left) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_3].current[1] += mesh.nodesVector.nodes[nodeID_3].rho *
				v2 * (x1 - left) * (top - x2) / hSquared;
		}
		else if (firstNodePosition == "TL")
		{
			mesh.nodesVector.nodes[nodeID_0].current[0] += mesh.nodesVector.nodes[nodeID_0].rho * 
				v1 * (right - x1) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_0].current[1] += mesh.nodesVector.nodes[nodeID_0].rho *
				v2 * (right - x1) * (x2 - bottom) / hSquared;

			mesh.nodesVector.nodes[nodeID_1].current[0] += mesh.nodesVector.nodes[nodeID_1].rho * 
				v1 * (right - x1) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_1].current[1] += mesh.nodesVector.nodes[nodeID_1].rho *
				v2 * (right - x1) * (top - x2) / hSquared;

			mesh.nodesVector.nodes[nodeID_2].current[0] += mesh.nodesVector.nodes[nodeID_2].rho * 
				v1 * (x1 - left) * (top - x2) / hSquared;
			mesh.nodesVector.nodes[nodeID_2].current[1] += mesh.nodesVector.nodes[nodeID_2].rho *
				v2 * (x1 - left) * (top - x2) / hSquared;

			mesh.nodesVector.nodes[nodeID_3].current[0] += mesh.nodesVector.nodes[nodeID_3].rho * 
				v1 * (x1 - left) * (x2 - bottom) / hSquared;
			mesh.nodesVector.nodes[nodeID_3].current[1] += mesh.nodesVector.nodes[nodeID_3].rho *
				v2 * (x1 - left) * (x2 - bottom) / hSquared;
		}
	}
	parametersList.logBrief("Charge projector exited", 1);
}