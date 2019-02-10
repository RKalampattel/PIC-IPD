//! \file
//! \brief Implementation of Interpolator method 
//! \author Rahul Kalampattel
//! \date Last updated February 2019

#include "Patch.h"

// TODO: Large parts of the code below are similar to what is in ChargeProjector.
// potentially look at making both functions of Patch, enabling code re-use?

// Interpolate quantities from mesh to particle locations
void Patch::Interpolator()
{
	listOfParticles.clearFields();
	
	double hSquared = mesh.h * mesh.h;

	// # pragma omp parallel for num_threads(parametersList.numThreads)
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

		if (firstNodePosition == "TL")
		{
			for (int j = 0; j < 6; j++)
			{
				particle.EMfield[j] =
					mesh.nodesVector.nodes[nodeID_0].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].EMfield[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].EMfield[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared;
			}
		}
		else if (firstNodePosition == "BL")
		{
			for (int j = 0; j < 6; j++)
			{
				particle.EMfield[j] =
					mesh.nodesVector.nodes[nodeID_0].EMfield[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].EMfield[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared;
			}
		}
		else if (firstNodePosition == "BR")
		{
			for (int j = 0; j < 6; j++)
			{
				particle.EMfield[j] =
					mesh.nodesVector.nodes[nodeID_0].EMfield[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].EMfield[j] * (right - x1) * (top - x2) / hSquared;
			}
		}
		else if (firstNodePosition == "TR")
		{
			for (int j = 0; j < 6; j++)
			{
				particle.EMfield[j] =
					mesh.nodesVector.nodes[nodeID_0].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].EMfield[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].EMfield[j] * (x1 - left) * (top - x2) / hSquared;
			}
		}
	}
	parametersList.logBrief("Field interpolator exited", 1);
}