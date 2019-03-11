//! \file
//! \brief Implementation of Interpolator method 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#include "Patch.h"

// Interpolate quantities from mesh to particle locations
void Patch::Interpolator()
{
	listOfParticles.clearFields();
	
	double hSquared = mesh.h * mesh.h;

	// # pragma omp parallel for num_threads(parametersList.numThreads)
	for (Particle& particle : listOfParticles.listOfParticles)
	{
		getIntermediateValues(particle);

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