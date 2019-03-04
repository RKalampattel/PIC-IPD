//! \file
//! \brief Implementation of FDTD method 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#include "Patch.h"

// Pre-processing for FDTD mesh
void Patch::FDTDpreprocess()
{
	int currentCol = 0, cellShift = 0;
	int step = static_cast<int>(round(mesh.h / FDTDmesh.h));

	// Check that the FDTD mesh spacing divides the PIC mesh spacing evenly
	if (step > 1 && abs(mesh.h - (FDTDmesh.h * round(mesh.h / FDTDmesh.h))) < 1e-10)
	{
		// Map nodes of the FDTD mesh to cells of the PIC mesh 	
		for (int i = 0; i < FDTDmesh.numNodes; i += (FDTDmesh.numRows + 1))
		{
			int currentRow = 1;
			if (currentCol == step + 1)
			{
				cellShift += mesh.numRows;
				currentCol = 1;
			}
			int currentCell = 1 + cellShift;

			FDTDmesh.nodesVector.nodes[i].PICcellID = currentCell;

			int j = 1;
			while (currentRow < FDTDmesh.numRows + 1)
			{
				for (int k = 1; k <= step; k++)
				{
					FDTDmesh.nodesVector.nodes[i + j].PICcellID = currentCell;
					j++;
					currentRow++;
				}
				currentCell++;
			}
			currentCol++;
		}
	}
	else
	{
		parametersList.logBrief("FDTD spacing should be a factor of PIC spacing", 3);
	}
}


// Resolves an external EM field using the FDTD method
void Patch::FDTD()
{
	// Based on the geometry of the existing mesh, generate a new mesh with a 
	// finer grid spacing. Nodes in every second row and second column are used 
	// to calculate E field parameters, while the alternate nodes are used for 
	// B field parameters (i.e a Yee mesh). 

	double hSquared = mesh.h * mesh.h;

	for (int i = 0; i < FDTDmesh.numNodes; i++)
	{
		// TODO: Can change all of the below to references to avoid copying large 
		// amounts of data for each calculation

		int cellID = FDTDmesh.nodesVector.nodes[i].PICcellID - 1;
		int nodeID_0 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[0] - 1;
		int nodeID_1 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[1] - 1;
		int nodeID_2 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[2] - 1;
		int nodeID_3 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[3] - 1;

		double left = mesh.cellsVector.cells[cellID].left;
		double right = mesh.cellsVector.cells[cellID].right;
		double top = mesh.cellsVector.cells[cellID].top;
		double bottom = mesh.cellsVector.cells[cellID].bottom;

		double x1 = FDTDmesh.nodesVector.nodes[i].geometry.X.element(0, 0);
		double x2 = FDTDmesh.nodesVector.nodes[i].geometry.X.element(1, 0);

		std::string firstNodePosition = mesh.cellsVector.cells[cellID].firstNodePosition;

		if (firstNodePosition == "TL")
		{
			for (int j = 0; j < 6; j++)
			{
				FDTDmesh.nodesVector.nodes[i].EMfield[j] =
					mesh.nodesVector.nodes[nodeID_0].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].EMfield[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].EMfield[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared;
			}
			for (int j = 0; j < 2; j++)
			{
				FDTDmesh.nodesVector.nodes[i].current[j] =
					mesh.nodesVector.nodes[nodeID_0].current[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].current[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].current[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].current[j] * (x1 - left) * (x2 - bottom) / hSquared;
			}
		}
		else if (firstNodePosition == "BL")
		{
			for (int j = 0; j < 6; j++)
			{
				FDTDmesh.nodesVector.nodes[i].EMfield[j] =
					mesh.nodesVector.nodes[nodeID_0].EMfield[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].EMfield[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared;
			}
			for (int j = 0; j < 2; j++)
			{
				FDTDmesh.nodesVector.nodes[i].current[j] =
					mesh.nodesVector.nodes[nodeID_0].current[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].current[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].current[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].current[j] * (right - x1) * (x2 - bottom) / hSquared;
			}
		}
		else if (firstNodePosition == "BR")
		{
			for (int j = 0; j < 6; j++)
			{
				FDTDmesh.nodesVector.nodes[i].EMfield[j] =
					mesh.nodesVector.nodes[nodeID_0].EMfield[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].EMfield[j] * (right - x1) * (top - x2) / hSquared;
			}
			for (int j = 0; j < 2; j++)
			{
				FDTDmesh.nodesVector.nodes[i].current[j] =
					mesh.nodesVector.nodes[nodeID_0].current[j] * (x1 - left) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].current[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].current[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].current[j] * (right - x1) * (top - x2) / hSquared;
			}
		}
		else if (firstNodePosition == "TR")
		{
			for (int j = 0; j < 6; j++)
			{
				FDTDmesh.nodesVector.nodes[i].EMfield[j] =
					mesh.nodesVector.nodes[nodeID_0].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].EMfield[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].EMfield[j] * (x1 - left) * (top - x2) / hSquared;
			}
			for (int j = 0; j < 2; j++)
			{
				FDTDmesh.nodesVector.nodes[i].current[j] =
					mesh.nodesVector.nodes[nodeID_0].current[j] * (x1 - left) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_1].current[j] * (right - x1) * (x2 - bottom) / hSquared +
					mesh.nodesVector.nodes[nodeID_2].current[j] * (right - x1) * (top - x2) / hSquared +
					mesh.nodesVector.nodes[nodeID_3].current[j] * (x1 - left) * (top - x2) / hSquared;
			}
		}
	}

	double cSquared = 1.0 / (EPSILON_0 * MU_0);
	double FDTDtimeStep = parametersList.timeStep / static_cast<double>(parametersList.FDTDiterations);
	double timeStepRatio = FDTDtimeStep / (FDTDmesh.h * 2.0);

	// Check that FDTD time step and grid spacing meet stability conditions
	if ((sqrt(2.0) * (FDTDmesh.h * 2.0) / sqrt(cSquared)) < FDTDtimeStep)
	{
		double difference = FDTDtimeStep / (sqrt(2.0) * (FDTDmesh.h * 2.0) / sqrt(cSquared));
		parametersList.logBrief("FDTD stability criterion exceeded by factor of " + std::to_string((int)difference), 2);
	}

	// Solve Maxwell's equations
	for (int i = 0; i < parametersList.FDTDiterations; i++)
	{
		// Solve B field equations based on E 
		# pragma omp parallel for num_threads(parametersList.numThreads)
		for (int j = 0; j < FDTDmesh.numNodes; j += 2)
		{
			int leftNodeID = FDTDmesh.nodesVector.nodes[j].leftNodeID - 1;
			int rightNodeID = FDTDmesh.nodesVector.nodes[j].rightNodeID - 1;
			int topNodeID = FDTDmesh.nodesVector.nodes[j].topNodeID - 1;
			int bottomNodeID = FDTDmesh.nodesVector.nodes[j].bottomNodeID - 1;

			if (FDTDmesh.nodesVector.nodes[j].boundaryType == "internal")
			{
				// (1) d/dt(Bx) = -d/dy(Ez)
				FDTDmesh.nodesVector.nodes[j].EMfield[3] -= timeStepRatio *
					(FDTDmesh.nodesVector.nodes[topNodeID].EMfield[2] - 
						FDTDmesh.nodesVector.nodes[bottomNodeID].EMfield[2]);
				// (2) d/dt(By) = d/dx(Ez)
				FDTDmesh.nodesVector.nodes[j].EMfield[4] += timeStepRatio *
					(FDTDmesh.nodesVector.nodes[rightNodeID].EMfield[2] -
						FDTDmesh.nodesVector.nodes[leftNodeID].EMfield[2]);
				// (3) d/dt(Bz) = -d/dx(Ey) + d/dy(Ex)
				FDTDmesh.nodesVector.nodes[j].EMfield[5] += timeStepRatio *
					(FDTDmesh.nodesVector.nodes[topNodeID].EMfield[0] -
						FDTDmesh.nodesVector.nodes[bottomNodeID].EMfield[0] - 
						FDTDmesh.nodesVector.nodes[rightNodeID].EMfield[1] +
						FDTDmesh.nodesVector.nodes[leftNodeID].EMfield[1]);
			}
			// TODO: Cases for boundary nodes
		}

		// Solve E field equations based on B
		# pragma omp parallel for num_threads(parametersList.numThreads)
		for (int j = 1; j < FDTDmesh.numNodes; j += 2)
		{
			int leftNodeID = FDTDmesh.nodesVector.nodes[j].leftNodeID - 1;
			int rightNodeID = FDTDmesh.nodesVector.nodes[j].rightNodeID - 1;
			int topNodeID = FDTDmesh.nodesVector.nodes[j].topNodeID - 1;
			int bottomNodeID = FDTDmesh.nodesVector.nodes[j].bottomNodeID - 1;

			if (FDTDmesh.nodesVector.nodes[j].boundaryType == "internal")
			{
				// (4) e.d/dt(Ex) = (1/u).d/dy(Bz) - Jx
				FDTDmesh.nodesVector.nodes[j].EMfield[0] += timeStepRatio * cSquared *
					(FDTDmesh.nodesVector.nodes[topNodeID].EMfield[5] -
						FDTDmesh.nodesVector.nodes[bottomNodeID].EMfield[5]) - 
					FDTDmesh.nodesVector.nodes[j].current[0] / EPSILON_0;
				// (5) e.d/dt(Ey) = -(1/u).d/dx(Bz) - Jy
				FDTDmesh.nodesVector.nodes[j].EMfield[1] -= timeStepRatio * cSquared *
					(FDTDmesh.nodesVector.nodes[rightNodeID].EMfield[5] -
						FDTDmesh.nodesVector.nodes[leftNodeID].EMfield[5]) - 
					FDTDmesh.nodesVector.nodes[j].current[1] / EPSILON_0;
				// (6) e.d/dt(Ez) = (1/u).d/dx(By) - (1/u).d/dy(Bx) 
				FDTDmesh.nodesVector.nodes[j].EMfield[2] += timeStepRatio * cSquared *
					(FDTDmesh.nodesVector.nodes[rightNodeID].EMfield[4] -
						FDTDmesh.nodesVector.nodes[leftNodeID].EMfield[4] -
					    FDTDmesh.nodesVector.nodes[topNodeID].EMfield[3] +
						FDTDmesh.nodesVector.nodes[bottomNodeID].EMfield[3]);
			}
			// TODO: Cases for boundary nodes
		}
	}

	for (int i = 0; i < FDTDmesh.numNodes; i++)
	{
		// TODO: Can change all of the below to references to avoid copying large 
		// amounts of data for each calculation

		int cellID = FDTDmesh.nodesVector.nodes[i].PICcellID - 1;
		int nodeID_0 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[0] - 1;
		int nodeID_1 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[1] - 1;
		int nodeID_2 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[2] - 1;
		int nodeID_3 = mesh.cellsVector.cells[cellID].connectivity.nodeIDs[3] - 1;

		double left = mesh.cellsVector.cells[cellID].left;
		double right = mesh.cellsVector.cells[cellID].right;
		double top = mesh.cellsVector.cells[cellID].top;
		double bottom = mesh.cellsVector.cells[cellID].bottom;

		double x1 = FDTDmesh.nodesVector.nodes[i].geometry.X.element(0, 0);
		double x2 = FDTDmesh.nodesVector.nodes[i].geometry.X.element(1, 0);

		std::string firstNodePosition = mesh.cellsVector.cells[cellID].firstNodePosition;

		if (firstNodePosition == "TL")
		{
			for (int j = 0; j < 6; j++)
			{
				mesh.nodesVector.nodes[nodeID_0].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared;
				mesh.nodesVector.nodes[nodeID_1].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (right - x1) * (top - x2) / hSquared;
				mesh.nodesVector.nodes[nodeID_2].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (x1 - left) * (top - x2) / hSquared;
				mesh.nodesVector.nodes[nodeID_3].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared;
			}
		}
		else if (firstNodePosition == "BL")
		{
			for (int j = 0; j < 6; j++)
			{
				mesh.nodesVector.nodes[nodeID_0].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (right - x1) * (top - x2) / hSquared;
				mesh.nodesVector.nodes[nodeID_1].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (x1 - left) * (top - x2) / hSquared;
				mesh.nodesVector.nodes[nodeID_2].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared;
				mesh.nodesVector.nodes[nodeID_3].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared;
			}
		}
		else if (firstNodePosition == "BR")
		{
			for (int j = 0; j < 6; j++)
			{
				mesh.nodesVector.nodes[nodeID_0].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (x1 - left) * (top - x2) / hSquared;
				mesh.nodesVector.nodes[nodeID_1].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared;
				mesh.nodesVector.nodes[nodeID_2].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared;
				mesh.nodesVector.nodes[nodeID_3].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (right - x1) * (top - x2) / hSquared;
			}
		}
		else if (firstNodePosition == "TR")
		{
			for (int j = 0; j < 6; j++)
			{
				mesh.nodesVector.nodes[nodeID_0].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (x1 - left) * (x2 - bottom) / hSquared;
				mesh.nodesVector.nodes[nodeID_1].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (right - x1) * (x2 - bottom) / hSquared;
				mesh.nodesVector.nodes[nodeID_2].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (right - x1) * (top - x2) / hSquared;
				mesh.nodesVector.nodes[nodeID_3].EMfield[j] += 
					FDTDmesh.nodesVector.nodes[i].EMfield[j] * (x1 - left) * (top - x2) / hSquared;
			}
		}
	}

	parametersList.logBrief("FDTD solver exited", 1);
}