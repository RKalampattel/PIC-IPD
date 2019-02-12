//! \file
//! \brief Implementation of Mesh class 
//! \author Rahul Kalampattel
//! \date Last updated February 2019

#include "Parameters.h"
#include "Mesh.h"

// Default constructor
Mesh::Mesh()
{
}

// TODO: Pre-compute node/cell volumes, V=A for Cartesian, else depends on position
// Constructor
Mesh::Mesh(Parameters *localParametersList, std::string type)
{
	if (type == "PIC")
	{
		localParametersList->logMessages("Starting additional mesh pre-processing", __FILENAME__, __LINE__, 1);

		// Extract data from gridinfo
		numCells = localParametersList->gridinfoPIC.NCM;
		numFaces = localParametersList->gridinfoPIC.NFM;
		numNodes = localParametersList->gridinfoPIC.NNM;
		dimension = localParametersList->gridinfoPIC.DIM;

		// Extract (vector) data from gridgeo
		cellsVector.allocate(localParametersList->gridgeoPIC.cells);
		facesVector.allocate(localParametersList->gridgeoPIC.faces);
		nodesVector.allocate(localParametersList->gridgeoPIC.nodes);
	}
	else if (type == "FDTD")
	{
		localParametersList->logBrief("Starting FDTD mesh pre-processing", 1);

		// Extract data from gridinfo
		numCells = localParametersList->gridinfoFDTD.NCM;
		numFaces = localParametersList->gridinfoFDTD.NFM;
		numNodes = localParametersList->gridinfoFDTD.NNM;
		dimension = localParametersList->gridinfoFDTD.DIM;

		// Extract (vector) data from gridgeo
		cellsVector.allocate(localParametersList->gridgeoFDTD.cells);
		facesVector.allocate(localParametersList->gridgeoFDTD.faces);
		nodesVector.allocate(localParametersList->gridgeoFDTD.nodes);
	}
	else
	{
		localParametersList->logBrief("Invalid type", 3);
	}

	minimumParticlesPerCell = localParametersList->minimumParticlesPerCell;
	double hAverage = 0;

	// Scale mesh
	// # pragma omp parallel for num_threads(localParametersList->numThreads)
	for (int i = 0; i < numNodes; i++)
	{
		nodesVector.nodes[i].geometry.X *= localParametersList->meshScalingParameter;
	}

	// Find boundaries of each cell and identify adjacent/neighbouring cells,
	// also identify connected nodes
	//# pragma omp parallel for num_threads(localParametersList->numThreads)
	for (int i = 0; i < numCells; i++)
	{
		// Only need to check two opposite nodes to find cell boundaries, and 
		// since nodes are ordered anticlockwise, this is satisfied by the first
		// and third node, hence the j indices used
		for (int j = 0; j < 3; j += 2)
		{
			// Node vector is indexed from 0, nodeIDs from 1, hence the index shift (-1) below
			int nodeID = cellsVector.cells[i].connectivity.nodeIDs[j] - 1;
			double x1 = nodesVector.nodes[nodeID].geometry.X.element(0, 0);
			double x2 = nodesVector.nodes[nodeID].geometry.X.element(1, 0);

			// Identify cell boundaries
			if (j == 0)
			{
				cellsVector.cells[i].left = x1;
				cellsVector.cells[i].right = x1;
				cellsVector.cells[i].top = x2;
				cellsVector.cells[i].bottom = x2;
			}
			else
			{
				if (x1 < cellsVector.cells[i].left)
				{
					cellsVector.cells[i].left = x1;
				}
				else if (x1 > cellsVector.cells[i].right)
				{
					cellsVector.cells[i].right = x1;
				}
				if (x2 < cellsVector.cells[i].bottom)
				{
					cellsVector.cells[i].bottom = x2;
				}
				else if (x2 > cellsVector.cells[i].top)
				{
					cellsVector.cells[i].top = x2;
				}
			}
		}

		hAverage += cellsVector.cells[i].right - cellsVector.cells[i].left + 
			cellsVector.cells[i].top - cellsVector.cells[i].bottom;

		// Get position of first node
		int nodeID1 = cellsVector.cells[i].connectivity.nodeIDs[0] - 1;
		int nodeID2 = cellsVector.cells[i].connectivity.nodeIDs[1] - 1;
		int nodeID3 = cellsVector.cells[i].connectivity.nodeIDs[2] - 1;
		int nodeID4 = cellsVector.cells[i].connectivity.nodeIDs[3] - 1;
		double x1 = nodesVector.nodes[nodeID1].geometry.X.element(0, 0);
		double x2 = nodesVector.nodes[nodeID1].geometry.X.element(1, 0);

		// IDs of faces and their respective adjacent cells
		int faceID_1 = cellsVector.cells[i].connectivity.faceIDs[0] - 1;
		int leftCell_1 = facesVector.faces[faceID_1].connectivity.cl[0] - 1;
		int rightCell_1 = facesVector.faces[faceID_1].connectivity.cr[0] - 1;

		int faceID_2 = cellsVector.cells[i].connectivity.faceIDs[1] - 1;
		int leftCell_2 = facesVector.faces[faceID_2].connectivity.cl[0] - 1;
		int rightCell_2 = facesVector.faces[faceID_2].connectivity.cr[0] - 1;

		int faceID_3 = cellsVector.cells[i].connectivity.faceIDs[2] - 1;
		int leftCell_3 = facesVector.faces[faceID_3].connectivity.cl[0] - 1;
		int rightCell_3 = facesVector.faces[faceID_3].connectivity.cr[0] - 1;

		int faceID_4 = cellsVector.cells[i].connectivity.faceIDs[3] - 1;
		int leftCell_4 = facesVector.faces[faceID_4].connectivity.cl[0] - 1;
		int rightCell_4 = facesVector.faces[faceID_4].connectivity.cr[0] - 1;

		// Identify position of adjacent cells based on location of first node,
		// identify connected nodes, and positions of cells and nodes w.r.t.
		// boundary of simulation domain
		if (x1 == cellsVector.cells[i].left && x2 == cellsVector.cells[i].top)		// Top left node
		{
			cellsVector.cells[i].firstNodePosition = "TL";
			cellsVector.cells[i].leftCellID = leftCell_1 + rightCell_1 - i + 1;
			cellsVector.cells[i].bottomCellID = leftCell_2 + rightCell_2 - i + 1;
			cellsVector.cells[i].rightCellID = leftCell_3 + rightCell_3 - i + 1;
			cellsVector.cells[i].topCellID = leftCell_4 + rightCell_4 - i + 1;

			nodesVector.nodes[nodeID1].bottomNodeID = nodeID2 + 1;
			nodesVector.nodes[nodeID1].rightNodeID = nodeID4 + 1;
			nodesVector.nodes[nodeID2].rightNodeID = nodeID3 + 1;
			nodesVector.nodes[nodeID2].topNodeID = nodeID1 + 1;
			nodesVector.nodes[nodeID3].topNodeID = nodeID4 + 1;
			nodesVector.nodes[nodeID3].leftNodeID = nodeID2 + 1;
			nodesVector.nodes[nodeID4].leftNodeID = nodeID1 + 1;
			nodesVector.nodes[nodeID4].bottomNodeID = nodeID3 + 1;

			if (cellsVector.cells[i].leftCellID > 0 && cellsVector.cells[i].rightCellID > 0 &&
				cellsVector.cells[i].topCellID > 0 && cellsVector.cells[i].bottomCellID > 0)
			{
				cellsVector.cells[i].boundaryType = "internal";
				nodesVector.nodes[nodeID1].boundaryType = "internal";
				nodesVector.nodes[nodeID2].boundaryType = "internal";
				nodesVector.nodes[nodeID3].boundaryType = "internal";
				nodesVector.nodes[nodeID4].boundaryType = "internal";
			}
			else if (cellsVector.cells[i].leftCellID < 1 && cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "BL";
				nodesVector.nodes[nodeID1].boundaryType = "L";
				nodesVector.nodes[nodeID2].boundaryType = "BL";
				nodesVector.nodes[nodeID3].boundaryType = "B";
				numRows = i + 1;
				numColumns = numCells / numRows;
			}
			else if (cellsVector.cells[i].leftCellID < 1 && cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "TL";
				nodesVector.nodes[nodeID1].boundaryType = "TL";
				nodesVector.nodes[nodeID2].boundaryType = "L";
				nodesVector.nodes[nodeID4].boundaryType = "T";
			}
			else if (cellsVector.cells[i].rightCellID < 1 && cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "TR";
				nodesVector.nodes[nodeID1].boundaryType = "T";
				nodesVector.nodes[nodeID3].boundaryType = "R";
				nodesVector.nodes[nodeID4].boundaryType = "TR";
			}
			else if (cellsVector.cells[i].rightCellID < 1 && cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "BR";
				nodesVector.nodes[nodeID2].boundaryType = "B";
				nodesVector.nodes[nodeID3].boundaryType = "BR";
				nodesVector.nodes[nodeID4].boundaryType = "R";
			}
			else if (cellsVector.cells[i].leftCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "L";
				nodesVector.nodes[nodeID1].boundaryType = "L";
				nodesVector.nodes[nodeID2].boundaryType = "L";
			}
			else if (cellsVector.cells[i].rightCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "R";
				nodesVector.nodes[nodeID3].boundaryType = "R";
				nodesVector.nodes[nodeID4].boundaryType = "R";
			}
			else if (cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "B";
				nodesVector.nodes[nodeID2].boundaryType = "B";
				nodesVector.nodes[nodeID3].boundaryType = "B";
			}
			else if (cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "T";
				nodesVector.nodes[nodeID1].boundaryType = "T";
				nodesVector.nodes[nodeID4].boundaryType = "T";
			}
		}
		else if (x1 == cellsVector.cells[i].left && x2 == cellsVector.cells[i].bottom)			// Bottom left node
		{
			cellsVector.cells[i].firstNodePosition = "BL";
			cellsVector.cells[i].bottomCellID = leftCell_1 + rightCell_1 - i + 1;
			cellsVector.cells[i].rightCellID = leftCell_2 + rightCell_2 - i + 1;
			cellsVector.cells[i].topCellID = leftCell_3 + rightCell_3 - i + 1;
			cellsVector.cells[i].leftCellID = leftCell_4 + rightCell_4 - i + 1;

			nodesVector.nodes[nodeID1].rightNodeID = nodeID2 + 1;
			nodesVector.nodes[nodeID1].topNodeID = nodeID4 + 1;
			nodesVector.nodes[nodeID2].topNodeID = nodeID3 + 1;
			nodesVector.nodes[nodeID2].leftNodeID = nodeID1 + 1;
			nodesVector.nodes[nodeID3].leftNodeID = nodeID4 + 1;
			nodesVector.nodes[nodeID3].bottomNodeID = nodeID2 + 1;
			nodesVector.nodes[nodeID4].bottomNodeID = nodeID1 + 1;
			nodesVector.nodes[nodeID4].rightNodeID = nodeID3 + 1;

			if (cellsVector.cells[i].leftCellID > 0 && cellsVector.cells[i].rightCellID > 0 &&
				cellsVector.cells[i].topCellID > 0 && cellsVector.cells[i].bottomCellID > 0)
			{
				cellsVector.cells[i].boundaryType = "internal";
				nodesVector.nodes[nodeID1].boundaryType = "internal";
				nodesVector.nodes[nodeID2].boundaryType = "internal";
				nodesVector.nodes[nodeID3].boundaryType = "internal";
				nodesVector.nodes[nodeID4].boundaryType = "internal";
			}
			else if (cellsVector.cells[i].leftCellID < 1 && cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "BL";
				nodesVector.nodes[nodeID1].boundaryType = "BL";
				nodesVector.nodes[nodeID2].boundaryType = "B";
				nodesVector.nodes[nodeID4].boundaryType = "L";
			}
			else if (cellsVector.cells[i].leftCellID < 1 && cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "TL";
				nodesVector.nodes[nodeID1].boundaryType = "L";
				nodesVector.nodes[nodeID3].boundaryType = "T";
				nodesVector.nodes[nodeID4].boundaryType = "TL";
			}
			else if (cellsVector.cells[i].rightCellID < 1 && cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "TR";
				nodesVector.nodes[nodeID2].boundaryType = "R";
				nodesVector.nodes[nodeID3].boundaryType = "TR";
				nodesVector.nodes[nodeID4].boundaryType = "T";
			}
			else if (cellsVector.cells[i].rightCellID < 1 && cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "BR";
				nodesVector.nodes[nodeID1].boundaryType = "B";
				nodesVector.nodes[nodeID2].boundaryType = "BR";
				nodesVector.nodes[nodeID3].boundaryType = "R";
			}
			else if (cellsVector.cells[i].leftCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "L";
				nodesVector.nodes[nodeID1].boundaryType = "L";
				nodesVector.nodes[nodeID4].boundaryType = "L";
			}
			else if (cellsVector.cells[i].rightCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "R";
				nodesVector.nodes[nodeID2].boundaryType = "R";
				nodesVector.nodes[nodeID3].boundaryType = "R";
			}
			else if (cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "B";
				nodesVector.nodes[nodeID1].boundaryType = "B";
				nodesVector.nodes[nodeID2].boundaryType = "B";
			}
			else if (cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "T";
				nodesVector.nodes[nodeID3].boundaryType = "T";
				nodesVector.nodes[nodeID4].boundaryType = "T";
			}
		}
		else if (x1 == cellsVector.cells[i].right && x2 == cellsVector.cells[i].bottom)	// Bottom right node
		{
			cellsVector.cells[i].firstNodePosition = "BR";
			cellsVector.cells[i].rightCellID = leftCell_1 + rightCell_1 - i + 1;
			cellsVector.cells[i].topCellID = leftCell_2 + rightCell_2 - i + 1;
			cellsVector.cells[i].leftCellID = leftCell_3 + rightCell_3 - i + 1;
			cellsVector.cells[i].bottomCellID = leftCell_4 + rightCell_4 - i + 1;

			nodesVector.nodes[nodeID1].topNodeID = nodeID2 + 1;
			nodesVector.nodes[nodeID1].leftNodeID = nodeID4 + 1;
			nodesVector.nodes[nodeID2].leftNodeID = nodeID3 + 1;
			nodesVector.nodes[nodeID2].bottomNodeID = nodeID1 + 1;
			nodesVector.nodes[nodeID3].bottomNodeID = nodeID4 + 1;
			nodesVector.nodes[nodeID3].rightNodeID = nodeID2 + 1;
			nodesVector.nodes[nodeID4].rightNodeID = nodeID1 + 1;
			nodesVector.nodes[nodeID4].topNodeID = nodeID3 + 1;

			if (cellsVector.cells[i].leftCellID > 0 && cellsVector.cells[i].rightCellID > 0 &&
				cellsVector.cells[i].topCellID > 0 && cellsVector.cells[i].bottomCellID > 0)
			{
				cellsVector.cells[i].boundaryType = "internal";
				nodesVector.nodes[nodeID1].boundaryType = "internal";
				nodesVector.nodes[nodeID2].boundaryType = "internal";
				nodesVector.nodes[nodeID3].boundaryType = "internal";
				nodesVector.nodes[nodeID4].boundaryType = "internal";
			}
			else if (cellsVector.cells[i].leftCellID < 1 && cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "BL";
				nodesVector.nodes[nodeID1].boundaryType = "B";
				nodesVector.nodes[nodeID3].boundaryType = "L";
				nodesVector.nodes[nodeID4].boundaryType = "BL";
			}
			else if (cellsVector.cells[i].leftCellID < 1 && cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "TL";
				nodesVector.nodes[nodeID2].boundaryType = "T";
				nodesVector.nodes[nodeID3].boundaryType = "TL";
				nodesVector.nodes[nodeID4].boundaryType = "L";
			}
			else if (cellsVector.cells[i].rightCellID < 1 && cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "TR";
				nodesVector.nodes[nodeID1].boundaryType = "R";
				nodesVector.nodes[nodeID2].boundaryType = "TR";
				nodesVector.nodes[nodeID3].boundaryType = "T";
			}
			else if (cellsVector.cells[i].rightCellID < 1 && cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "BR";
				nodesVector.nodes[nodeID1].boundaryType = "BR";
				nodesVector.nodes[nodeID2].boundaryType = "R";
				nodesVector.nodes[nodeID4].boundaryType = "B";
			}
			else if (cellsVector.cells[i].leftCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "L";
				nodesVector.nodes[nodeID3].boundaryType = "L";
				nodesVector.nodes[nodeID4].boundaryType = "L";
			}
			else if (cellsVector.cells[i].rightCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "R";
				nodesVector.nodes[nodeID1].boundaryType = "R";
				nodesVector.nodes[nodeID2].boundaryType = "R";
			}
			else if (cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "B";
				nodesVector.nodes[nodeID1].boundaryType = "B";
				nodesVector.nodes[nodeID4].boundaryType = "B";
			}
			else if (cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "T";
				nodesVector.nodes[nodeID2].boundaryType = "T";
				nodesVector.nodes[nodeID3].boundaryType = "T";
			}
		}
		else if (x1 == cellsVector.cells[i].right && x2 == cellsVector.cells[i].top)		// Top right node
		{
			cellsVector.cells[i].firstNodePosition = "TR";
			cellsVector.cells[i].topCellID = leftCell_1 + rightCell_1 - i + 1;
			cellsVector.cells[i].leftCellID = leftCell_2 + rightCell_2 - i + 1;
			cellsVector.cells[i].bottomCellID = leftCell_3 + rightCell_3 - i + 1;
			cellsVector.cells[i].rightCellID = leftCell_4 + rightCell_4 - i + 1;

			nodesVector.nodes[nodeID1].leftNodeID = nodeID2 + 1;
			nodesVector.nodes[nodeID1].bottomNodeID = nodeID4 + 1;
			nodesVector.nodes[nodeID2].bottomNodeID = nodeID3 + 1;
			nodesVector.nodes[nodeID2].rightNodeID = nodeID1 + 1;
			nodesVector.nodes[nodeID3].rightNodeID = nodeID4 + 1;
			nodesVector.nodes[nodeID3].topNodeID = nodeID2 + 1;
			nodesVector.nodes[nodeID4].topNodeID = nodeID1 + 1;
			nodesVector.nodes[nodeID4].leftNodeID = nodeID3 + 1;

			if (cellsVector.cells[i].leftCellID > 0 && cellsVector.cells[i].rightCellID > 0 &&
				cellsVector.cells[i].topCellID > 0 && cellsVector.cells[i].bottomCellID > 0)
			{
				cellsVector.cells[i].boundaryType = "internal";
				nodesVector.nodes[nodeID1].boundaryType = "internal";
				nodesVector.nodes[nodeID2].boundaryType = "internal";
				nodesVector.nodes[nodeID3].boundaryType = "internal";
				nodesVector.nodes[nodeID4].boundaryType = "internal";
			}
			else if (cellsVector.cells[i].leftCellID < 1 && cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "BL";
				nodesVector.nodes[nodeID2].boundaryType = "L";
				nodesVector.nodes[nodeID3].boundaryType = "BL";
				nodesVector.nodes[nodeID4].boundaryType = "B";
			}
			else if (cellsVector.cells[i].leftCellID < 1 && cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "TL";
				nodesVector.nodes[nodeID1].boundaryType = "T";
				nodesVector.nodes[nodeID2].boundaryType = "TL";
				nodesVector.nodes[nodeID3].boundaryType = "L";
			}
			else if (cellsVector.cells[i].rightCellID < 1 && cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "TR";
				nodesVector.nodes[nodeID1].boundaryType = "TR";
				nodesVector.nodes[nodeID2].boundaryType = "T";
				nodesVector.nodes[nodeID4].boundaryType = "R";
			}
			else if (cellsVector.cells[i].rightCellID < 1 && cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "BR";
				nodesVector.nodes[nodeID1].boundaryType = "R";
				nodesVector.nodes[nodeID3].boundaryType = "B";
				nodesVector.nodes[nodeID4].boundaryType = "BR";
			}
			else if (cellsVector.cells[i].leftCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "L";
				nodesVector.nodes[nodeID2].boundaryType = "L";
				nodesVector.nodes[nodeID3].boundaryType = "L";
			}
			else if (cellsVector.cells[i].rightCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "R";
				nodesVector.nodes[nodeID1].boundaryType = "R";
				nodesVector.nodes[nodeID4].boundaryType = "R";
			}
			else if (cellsVector.cells[i].bottomCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "B";
				nodesVector.nodes[nodeID3].boundaryType = "B";
				nodesVector.nodes[nodeID4].boundaryType = "B";
			}
			else if (cellsVector.cells[i].topCellID < 1)
			{
				cellsVector.cells[i].boundaryType = "T";
				nodesVector.nodes[nodeID1].boundaryType = "T";
				nodesVector.nodes[nodeID2].boundaryType = "T";
			}
		}
	}

	h = hAverage / static_cast<double>((2 * numCells));
	if (type == "PIC" && (h - localParametersList->PICspacing) > 1e-10)
	{
		localParametersList->logBrief("Grid spacing not calculated correctly", 3);
	}
	else if (type == "FDTD" && (h - localParametersList->FDTDspacing) > 1e-10)
	{
		localParametersList->logBrief("Grid spacing not calculated correctly", 3);
	}


	// Find connected cells based on periodic BCs
	// # pragma omp parallel for num_threads(localParametersList->numThreads)
	for (int i = 0; i < numCells; i++)
	{
		// Periodic cells in Cartesian x/cylindrical z direction
		if (cellsVector.cells[i].boundaryType == "TL" || 
			cellsVector.cells[i].boundaryType == "L" || 
			cellsVector.cells[i].boundaryType == "BL")
		{
			int j = cellsVector.cells[i].rightCellID - 1;
			while (cellsVector.cells[j].boundaryType != "TR" && 
				cellsVector.cells[j].boundaryType != "R" && 
				cellsVector.cells[j].boundaryType != "BR")
			{
				j = cellsVector.cells[j].rightCellID - 1; 
			}
			cellsVector.cells[i].periodicX1CellID = j + 1;
			cellsVector.cells[j].periodicX1CellID = i + 1;
		}

		// TODO: Does it make physical sense to have periodic BCs in the y/r 
		// direction? Definitely not for cylindrical, not really for Cartesian...
		// Periodic cells in Cartesian y/cylindrical r direction
		if (cellsVector.cells[i].boundaryType == "TL" || 
			cellsVector.cells[i].boundaryType == "T" || 
			cellsVector.cells[i].boundaryType == "TR")
		{
			int j = cellsVector.cells[i].bottomCellID - 1;
			while (cellsVector.cells[j].boundaryType != "BL" && 
				cellsVector.cells[j].boundaryType != "B" && 
				cellsVector.cells[j].boundaryType != "BR")
			{
				j = cellsVector.cells[j].bottomCellID - 1;
			}
			cellsVector.cells[i].periodicX2CellID = j + 1;
			cellsVector.cells[j].periodicX2CellID = i + 1;
		}
	}


	// Find connected nodes based on periodic BCs
	//# pragma omp parallel for num_threads(localParametersList->numThreads)
	for (int i = 0; i < numNodes; i++)
	{
		// Periodic nodes in x direction
		if (nodesVector.nodes[i].boundaryType == "TL" ||
			nodesVector.nodes[i].boundaryType == "L" ||
			nodesVector.nodes[i].boundaryType == "BL")
		{
			int j = nodesVector.nodes[i].rightNodeID - 1;
			while (nodesVector.nodes[j].boundaryType != "TR" &&
				nodesVector.nodes[j].boundaryType != "R" &&
				nodesVector.nodes[j].boundaryType != "BR")
			{
				j = nodesVector.nodes[j].rightNodeID - 1;
			}
			nodesVector.nodes[i].periodicX1NodeID = j + 1;
			nodesVector.nodes[j].periodicX1NodeID = i + 1;
		}

		// Periodic nodes in y direction
		if (nodesVector.nodes[i].boundaryType == "TL" ||
			nodesVector.nodes[i].boundaryType == "T" ||
			nodesVector.nodes[i].boundaryType == "TR")
		{
			int j = nodesVector.nodes[i].bottomNodeID - 1;
			while (nodesVector.nodes[j].boundaryType != "BL" &&
				nodesVector.nodes[j].boundaryType != "B" &&
				nodesVector.nodes[j].boundaryType != "BR")
			{
				j = nodesVector.nodes[j].bottomNodeID - 1;
			}
			nodesVector.nodes[i].periodicX2NodeID = j + 1;
			nodesVector.nodes[j].periodicX2NodeID = i + 1;
		}
	}
	localParametersList->logBrief("Additional mesh pre-processing complete", 1);
}


// Destructor
Mesh::~Mesh()
{
}


// Assign particle IDs to a cell
void Mesh::addParticlesToCell(int cellID, int particleID)
{
	cellsVector.cells[cellID - 1].particlesInCell.push_back(particleID);
}


// Remove particle IDs from a cell
void Mesh::removeParticlesFromCell(int cellID, int particleID)
{
	for (int i = 0; i < cellsVector.cells[cellID - 1].particlesInCell.size(); i++)
	{
		if (cellsVector.cells[cellID - 1].particlesInCell[i] == particleID)
		{
			cellsVector.cells[cellID - 1].particlesInCell.erase(cellsVector.cells[cellID - 1].particlesInCell.begin() + i);
		}
	}
}


// Check particle density per cell
int Mesh::checkParticleDensity()
{
	int lowestParticleDensity = minimumParticlesPerCell;
	for (int i = 0; i < numCells; i++)
	{
		if (cellsVector.cells[i].particlesInCell.size() < lowestParticleDensity)
		{
			lowestParticleDensity = cellsVector.cells[i].particlesInCell.size();
		}
	}
	if (lowestParticleDensity < minimumParticlesPerCell)
	{
		return minimumParticlesPerCell - lowestParticleDensity;
	}
	else
	{
		return 0;
	}
}
