//! \file
//! \brief Implementation of Solver method 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#include "Patch.h"

#include "fftw3.h"

// Solves the Poisson equation
void Patch::Solver()
{
	// Set potential and fields at all nodes to zero at the start of each step
	mesh.nodesVector.clearPhi();
	mesh.nodesVector.clearFields();

	double h = mesh.h;

	// TODO: How to solve for phi when open BCs are present?
	
	// TODO: Develop a function to solve for different combinations of BCs,
	// rather than explicitly going through each case - otherwise, will need 4 
	// cases per side (16 total) plus 16 cases per corner (64 total), meaning
	// 81 total cases, which is too much work!!!

	// TODO: Test edge cases for all BC's 

	// TODO: Check that time step is fine enough for solver stability
	for (int i = 0; i < parametersList.maxSolverIterations; i++)
	{
		// Axisymmetric simulation
		if (parametersList.axisymmetric)
		{
			// TODO: Corner node equations

			for (int j = 0; j < mesh.numNodes; j++)
			{
				int leftNodeID = mesh.nodesVector.nodes[j].leftNodeID - 1;
				int rightNodeID = mesh.nodesVector.nodes[j].rightNodeID - 1;
				int topNodeID = mesh.nodesVector.nodes[j].topNodeID - 1;
				int bottomNodeID = mesh.nodesVector.nodes[j].bottomNodeID - 1;

				if (mesh.nodesVector.nodes[j].boundaryType == "internal")
				{
					mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
						((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
							mesh.nodesVector.nodes[leftNodeID].phi +
							mesh.nodesVector.nodes[rightNodeID].phi +
							(1.0 + h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
							mesh.nodesVector.nodes[topNodeID].phi +
							(1.0 - h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
							mesh.nodesVector.nodes[bottomNodeID].phi) +
							(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
				}
				else if (mesh.nodesVector.nodes[j].boundaryType == "L")
				{
					if (parametersList.leftBCType == "periodic")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
							((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
								mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].periodicX1NodeID - 1].phi +
								mesh.nodesVector.nodes[rightNodeID].phi +
								(1.0 + h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
								mesh.nodesVector.nodes[topNodeID].phi +
								(1.0 - h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
								mesh.nodesVector.nodes[bottomNodeID].phi) +
								(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
					}
					else if (parametersList.leftBCType == "dirichlet")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.leftBCValue;
					}
					else if (parametersList.leftBCType == "neumann")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * (1.0 / 3.0) *
							((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
								mesh.nodesVector.nodes[rightNodeID].phi +
								(1.0 + h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
								mesh.nodesVector.nodes[topNodeID].phi +
								(1.0 - h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
								mesh.nodesVector.nodes[bottomNodeID].phi -
								h * parametersList.leftBCValue) + (1 - parametersList.SORparameter) *
							mesh.nodesVector.nodes[j].phi;
					}
				}
				else if (mesh.nodesVector.nodes[j].boundaryType == "R")
				{
					if (parametersList.rightBCType == "periodic")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
							((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
								mesh.nodesVector.nodes[leftNodeID].phi +
								mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].periodicX1NodeID - 1].phi +
								(1.0 + h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
								mesh.nodesVector.nodes[topNodeID].phi +
								(1.0 - h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
								mesh.nodesVector.nodes[bottomNodeID].phi) +
								(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
					}
					else if (parametersList.rightBCType == "dirichlet")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.rightBCValue;
					}
					else if (parametersList.rightBCType == "neumann")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * (1.0 / 3.0) *
							((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
								mesh.nodesVector.nodes[leftNodeID].phi +
								(1.0 + h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
								mesh.nodesVector.nodes[topNodeID].phi +
								(1.0 - h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
								mesh.nodesVector.nodes[bottomNodeID].phi +
								h * parametersList.rightBCValue) + (1 - parametersList.SORparameter) *
							mesh.nodesVector.nodes[j].phi;
					}
				}
				else if (mesh.nodesVector.nodes[j].boundaryType == "T")
				{
					// Periodic y BCs not valid for axisymmetric simulation since 
					// bottom boundary is actually an axis 

					if (parametersList.topBCType == "dirichlet")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.topBCValue;
					}
					else if (parametersList.topBCType == "neumann")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * (1.0 / 3.0) *
							(h * h * ((mesh.nodesVector.nodes[j].rho / EPSILON_0) +
							(parametersList.topBCValue / mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) +
								mesh.nodesVector.nodes[leftNodeID].phi +
								mesh.nodesVector.nodes[rightNodeID].phi +
								mesh.nodesVector.nodes[bottomNodeID].phi +
								h * parametersList.topBCValue) + (1 - parametersList.SORparameter) *
							mesh.nodesVector.nodes[j].phi;
					}
				}
				else if (mesh.nodesVector.nodes[j].boundaryType == "B")
				{
					// Bottom nodes must always obey the symmetry BC

					mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
						((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
							mesh.nodesVector.nodes[leftNodeID].phi +
							mesh.nodesVector.nodes[rightNodeID].phi +
							2 * mesh.nodesVector.nodes[topNodeID].phi) +
							(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
				}
			}

			// Check convergence
			if (i != 0 && i % 9 == 0)
			{
				double residualSum = 0;

				for (int j = 0; j < mesh.numNodes; j++)
				{
					// TODO: Include other nodes in calculating residual sum (???)
					if (mesh.nodesVector.nodes[j].boundaryType == "internal")
					{
						double residual = 
							(mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
							mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].leftNodeID - 1].phi +
							mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].rightNodeID - 1].phi +
							(1.0 + h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
							mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].topNodeID - 1].phi +
							(1.0 - h / (2 * mesh.nodesVector.nodes[j].geometry.X.element(1, 0))) *
							mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].bottomNodeID - 1].phi -
							4 * mesh.nodesVector.nodes[j].phi;

						residualSum += residual * residual;
					}
				}

				if (sqrt(residualSum / static_cast<double>(mesh.numNodes)) < parametersList.residualTolerance)
				{
					parametersList.logBrief("Solver convergence criteria met", 1);
					break;
				}
			}

			// Account for periodic BCs 
			for (int i = 0; i < mesh.numNodes; i++)
			{
				if (parametersList.leftBCType == "periodic")
				{
					if (mesh.nodesVector.nodes[i].boundaryType == "TL" ||
						mesh.nodesVector.nodes[i].boundaryType == "L" ||
						mesh.nodesVector.nodes[i].boundaryType == "BL")
					{
						mesh.nodesVector.nodes[i].phi = 0.5 * (mesh.nodesVector.nodes[i].phi +
							mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX1NodeID - 1].phi);
						mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX1NodeID - 1].phi =
							mesh.nodesVector.nodes[i].phi;
					}
				}
			}
		}
		else
		{
			if (parametersList.solverType == "FFT")
			{
				// TODO: Implement FFT based solver for mixed BC cases, and until
				// then check that the same BC is applied across the domain
				int nx = mesh.numColumns + 1, ny = mesh.numRows + 1;

				// Periodic BC case
				if (parametersList.bottomBCType == "periodic" && parametersList.rightBCType == "periodic")
				{
					// Allocate memory for signal (real) and transformed signal (complex)
					double *signal;
					signal = (double*)fftw_malloc(sizeof(double) * mesh.numNodes);
					fftw_complex *transform;
					transform = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) *
						ny * (1 + nx / 2));

					// Copy charge density from nodesVector into signal array
					for (int i = 0; i < ny; i++)
					{
						for (int j = 0; j < nx; j++)
						{
							signal[i*nx + j] =
								mesh.nodesVector.nodes[i*nx + j].rho;
						}
					}

					// Create and execute plan for forwards DFT
					fftw_plan forwardsPlan = fftw_plan_dft_r2c_2d(ny, nx, signal,
						transform, FFTW_ESTIMATE);
					fftw_execute(forwardsPlan);

					// Calculate (transformed) potential phi based on (transformed)
					// charge density				
					double W = exp(2.0 * PI * sqrt(-1.0) / static_cast<double>(nx));
					double Wm = 1, Wn = 1;

					// TODO: Current formulation assumes uniform length boundaries
					for (int i = 0; i < nx; i++)
					{
						for (int j = 0; j < ny; j++)
						{
							double denominator = 4 - Wm - Wn - 1.0 / Wm - 1.0 / Wn;
							if (denominator != 0.0)
							{
								transform[i*ny + j][0] *= mesh.h * mesh.h / denominator;
								// TODO: Need to multiply complex part as well??
							}
							Wn *= W;
						}
						Wm *= W;
					}

					// Create and execute plan for backwards (inverse) DFT
					fftw_plan backwardsPlan = fftw_plan_dft_c2r_2d(ny,
						nx, transform, signal, FFTW_ESTIMATE);
					fftw_execute(backwardsPlan);

					// Write data from signal array back to nodesVector phi
					for (int i = 0; i < ny; i++)
					{
						for (int j = 0; j < nx; j++)
						{
							mesh.nodesVector.nodes[i*nx + j].phi = signal[i*nx + j] /
								static_cast<double>(ny * nx);
						}
					}

					// Destroy plans and memory blocks allocated with fftw_malloc
					fftw_destroy_plan(forwardsPlan);
					fftw_destroy_plan(backwardsPlan);
					fftw_free(signal);
					fftw_free(transform);
				}
				// Dirichlet BC case
				else if (parametersList.bottomBCType == "dirichlet" && parametersList.rightBCType == "dirichlet")
				{
					// Allocate memory for signal (real) and transformed signal (real)
					double *signal, *transform;
					signal = (double*)fftw_malloc(sizeof(double) * mesh.numNodes);
					transform = (double*)fftw_malloc(sizeof(double) * mesh.numNodes);

					// Copy charge density from nodesVector into signal array
					for (int i = 0; i < ny; i++)
					{
						for (int j = 0; j < nx; j++)
						{
							signal[i*nx + j] =
								mesh.nodesVector.nodes[i*nx + j].rho;
						}
					}

					// Create and execute plan for forwards DST
					fftw_plan forwardsPlan = fftw_plan_r2r_2d(ny, nx, signal,
						transform, FFTW_RODFT00, FFTW_RODFT00, FFTW_ESTIMATE);
					fftw_execute(forwardsPlan);

					// Calculate (transformed) potential phi based on (transformed)
					// charge density				

					// TODO: Current formulation assumes uniform length boundaries
					for (int i = 0; i < nx; i++)
					{
						for (int j = 0; j < ny; j++)
						{
							double denominator = 4.0 - 2.0 * 
								(cos(PI * static_cast<double>(i + 1) / static_cast<double>(nx + 1)) +
								cos(PI * static_cast<double>(j + 1) / static_cast<double>(ny + 1)));
							if (denominator != 0.0)
							{
								transform[i*ny + j] *= mesh.h * mesh.h / denominator;
							}
						}
					}

					// Create and execute plan for backwards (inverse) DST
					fftw_plan backwardsPlan = fftw_plan_r2r_2d(ny, nx, transform,
						signal, FFTW_RODFT00, FFTW_RODFT00, FFTW_ESTIMATE);
					fftw_execute(backwardsPlan);

					// Write data from signal array back to nodesVector phi
					for (int i = 0; i < ny; i++)
					{
						for (int j = 0; j < nx; j++)
						{
							mesh.nodesVector.nodes[i*nx + j].phi = signal[i*nx + j] /
								static_cast<double>((2.0 * (ny + 1.0)) * (2.0 * (nx + 1.0)));
						}
					}

					// Destroy plans and memory blocks allocated with fftw_malloc
					fftw_destroy_plan(forwardsPlan);
					fftw_destroy_plan(backwardsPlan);
					fftw_free(signal);
					fftw_free(transform);
				}
				// Neumann BC case
				else if (parametersList.bottomBCType == "neumann" && parametersList.rightBCType == "neumann")
				{
					// Allocate memory for signal (real) and transformed signal (real)
					double *signal, *transform;
					signal = (double*)fftw_malloc(sizeof(double) * mesh.numNodes);
					transform = (double*)fftw_malloc(sizeof(double) * mesh.numNodes);

					// Copy charge density from nodesVector into signal array
					for (int i = 0; i < ny; i++)
					{
						for (int j = 0; j < nx; j++)
						{
							signal[i*nx + j] =
								mesh.nodesVector.nodes[i*nx + j].rho;
						}
					}

					// Create and execute plan for forwards DCT
					fftw_plan forwardsPlan = fftw_plan_r2r_2d(ny, nx, signal,
						transform, FFTW_REDFT11, FFTW_REDFT11, FFTW_ESTIMATE);
					fftw_execute(forwardsPlan);

					// Calculate (transformed) potential phi based on (transformed)
					// charge density				

					// TODO: Current formulation assumes uniform length boundaries
					for (int i = 0; i < nx; i++)
					{
						for (int j = 0; j < ny; j++)
						{
							double denominator = 4.0 - 2.0 *
								(cos(PI * static_cast<double>(i + 0.5) / static_cast<double>(nx)) +
									cos(PI * static_cast<double>(j + 0.5) / static_cast<double>(ny)));
							if (denominator != 0.0)
							{
								transform[i*ny + j] *= mesh.h * mesh.h / denominator;
							}
						}
					}

					// Create and execute plan for backwards (inverse) DCT
					fftw_plan backwardsPlan = fftw_plan_r2r_2d(ny, nx, transform,
						signal, FFTW_REDFT11, FFTW_REDFT11, FFTW_ESTIMATE);
					fftw_execute(backwardsPlan);

					// Write data from signal array back to nodesVector phi
					for (int i = 0; i < ny; i++)
					{
						for (int j = 0; j < nx; j++)
						{
							mesh.nodesVector.nodes[i*nx + j].phi = signal[i*nx + j] /
								static_cast<double>((2.0 * ny) * (2.0 * nx));
						}
					}

					// Destroy plans and memory blocks allocated with fftw_malloc
					fftw_destroy_plan(forwardsPlan);
					fftw_destroy_plan(backwardsPlan);
					fftw_free(signal);
					fftw_free(transform);
				}
			}
			// Gauss-Seidel solver with successive over-relaxation (SOR)
			else if (parametersList.solverType == "GS")
			{
				for (int j = 0; j < mesh.numNodes; j++)
				{
					// Five point stencil is used on internal points and periodic
					// boundaries, while for other BCs and the four corner nodes, a
					// three point stencil or fixed value is used

					int leftNodeID = mesh.nodesVector.nodes[j].leftNodeID - 1;
					int rightNodeID = mesh.nodesVector.nodes[j].rightNodeID - 1;
					int topNodeID = mesh.nodesVector.nodes[j].topNodeID - 1;
					int bottomNodeID = mesh.nodesVector.nodes[j].bottomNodeID - 1;

					if (mesh.nodesVector.nodes[j].boundaryType == "internal")
					{
						mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
							((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
								mesh.nodesVector.nodes[leftNodeID].phi +
								mesh.nodesVector.nodes[rightNodeID].phi +
								mesh.nodesVector.nodes[topNodeID].phi +
								mesh.nodesVector.nodes[bottomNodeID].phi) +
								(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
					}
					else if (mesh.nodesVector.nodes[j].boundaryType == "L")
					{
						if (parametersList.leftBCType == "periodic")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
								((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
									mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].periodicX1NodeID - 1].phi +
									mesh.nodesVector.nodes[rightNodeID].phi +
									mesh.nodesVector.nodes[topNodeID].phi +
									mesh.nodesVector.nodes[bottomNodeID].phi) +
									(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
						}
						else if (parametersList.leftBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.leftBCValue;
						}
						else if (parametersList.leftBCType == "neumann")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * (1.0 / 3.0) *
								((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
									mesh.nodesVector.nodes[rightNodeID].phi +
									mesh.nodesVector.nodes[topNodeID].phi +
									mesh.nodesVector.nodes[bottomNodeID].phi -
									parametersList.leftBCValue * h) + (1 - parametersList.SORparameter) *
								mesh.nodesVector.nodes[j].phi;
						}
					}
					else if (mesh.nodesVector.nodes[j].boundaryType == "R")
					{
						if (parametersList.rightBCType == "periodic")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
								((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
									mesh.nodesVector.nodes[leftNodeID].phi +
									mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].periodicX1NodeID - 1].phi +
									mesh.nodesVector.nodes[topNodeID].phi +
									mesh.nodesVector.nodes[bottomNodeID].phi) +
									(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
						}
						else if (parametersList.rightBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.rightBCValue;
						}
						else if (parametersList.rightBCType == "neumann")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * (1.0 / 3.0) *
								((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
									mesh.nodesVector.nodes[leftNodeID].phi +
									mesh.nodesVector.nodes[topNodeID].phi +
									mesh.nodesVector.nodes[bottomNodeID].phi +
									parametersList.rightBCValue * h) + (1 - parametersList.SORparameter) *
								mesh.nodesVector.nodes[j].phi;
						}
					}
					else if (mesh.nodesVector.nodes[j].boundaryType == "T")
					{
						if (parametersList.topBCType == "periodic")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
								((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
									mesh.nodesVector.nodes[leftNodeID].phi +
									mesh.nodesVector.nodes[rightNodeID].phi +
									mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].periodicX2NodeID - 1].phi +
									mesh.nodesVector.nodes[bottomNodeID].phi) +
									(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
						}
						else if (parametersList.topBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.topBCValue;
						}
						else if (parametersList.topBCType == "neumann")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * (1.0 / 3.0) *
								((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
									mesh.nodesVector.nodes[leftNodeID].phi +
									mesh.nodesVector.nodes[rightNodeID].phi +
									mesh.nodesVector.nodes[bottomNodeID].phi +
									parametersList.topBCValue * h) + (1 - parametersList.SORparameter) *
								mesh.nodesVector.nodes[j].phi;
						}
					}
					else if (mesh.nodesVector.nodes[j].boundaryType == "B")
					{
						if (parametersList.bottomBCType == "periodic")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.25 *
								((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
									mesh.nodesVector.nodes[leftNodeID].phi +
									mesh.nodesVector.nodes[rightNodeID].phi +
									mesh.nodesVector.nodes[topNodeID].phi +
									mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].periodicX2NodeID - 1].phi) +
									(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
						}
						else if (parametersList.bottomBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.bottomBCValue;
						}
						else if (parametersList.bottomBCType == "neumann")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * (1.0 / 3.0) *
								((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
									mesh.nodesVector.nodes[leftNodeID].phi +
									mesh.nodesVector.nodes[rightNodeID].phi +
									mesh.nodesVector.nodes[topNodeID].phi -
									parametersList.bottomBCValue * h) + (1 - parametersList.SORparameter) *
								mesh.nodesVector.nodes[j].phi;
						}
					}
					else
					{
						if (parametersList.leftBCType == "neumann" &&
							parametersList.rightBCType == "neumann" &&
							parametersList.topBCType == "neumann" &&
							parametersList.bottomBCType == "neumann")
						{
							if (mesh.nodesVector.nodes[j].boundaryType == "TL")
							{
								mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.5 *
									((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
										mesh.nodesVector.nodes[rightNodeID].phi +
										mesh.nodesVector.nodes[bottomNodeID].phi +
										h * (parametersList.topBCValue - parametersList.leftBCValue)) +
										(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
							}
							else if (mesh.nodesVector.nodes[j].boundaryType == "BL")
							{
								mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.5 *
									((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
										mesh.nodesVector.nodes[rightNodeID].phi +
										mesh.nodesVector.nodes[topNodeID].phi -
										h * (parametersList.bottomBCValue + parametersList.leftBCValue)) +
										(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
							}
							else if (mesh.nodesVector.nodes[j].boundaryType == "TR")
							{
								mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.5 *
									((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
										mesh.nodesVector.nodes[leftNodeID].phi +
										mesh.nodesVector.nodes[bottomNodeID].phi +
										h * (parametersList.topBCValue + parametersList.rightBCValue)) +
										(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
							}
							else if (mesh.nodesVector.nodes[j].boundaryType == "BR")
							{
								mesh.nodesVector.nodes[j].phi = parametersList.SORparameter * 0.5 *
									((mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
										mesh.nodesVector.nodes[leftNodeID].phi +
										mesh.nodesVector.nodes[topNodeID].phi +
										h * (parametersList.rightBCValue - parametersList.bottomBCValue)) +
										(1 - parametersList.SORparameter) * mesh.nodesVector.nodes[j].phi;
							}
						}
						else if (parametersList.leftBCType == "dirichlet" &&
							parametersList.topBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = 0.5 * (parametersList.leftBCValue +
								parametersList.topBCValue);
						}
						else if (parametersList.leftBCType == "dirichlet" &&
							parametersList.bottomBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = 0.5 * (parametersList.leftBCValue +
								parametersList.bottomBCValue);
						}
						else if (parametersList.rightBCType == "dirichlet" &&
							parametersList.topBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = 0.5 * (parametersList.rightBCValue +
								parametersList.topBCValue);
						}
						else if (parametersList.rightBCType == "dirichlet" &&
							parametersList.bottomBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = 0.5 * (parametersList.rightBCValue +
								parametersList.bottomBCValue);
						}
						else if (parametersList.leftBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.leftBCValue;
						}
						else if (parametersList.rightBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.rightBCValue;
						}
						else if (parametersList.topBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.topBCValue;
						}
						else if (parametersList.bottomBCType == "dirichlet")
						{
							mesh.nodesVector.nodes[j].phi = parametersList.bottomBCValue;
						}
						else
						{
							// TODO: What happens for open BC cases???
							parametersList.logBrief("Unable to resolve corner BCs", 2);
							mesh.nodesVector.nodes[j].phi = 0.0;
						}
					}
				}

				// Check convergence
				if (i != 0 && i % 9 == 0)
				{
					double residualSum = 0;

					for (int j = 0; j < mesh.numNodes; j++)
					{
						// TODO: Include other nodes in calculating residual sum (???)
						if (mesh.nodesVector.nodes[j].boundaryType == "internal")
						{
							double residual =
								(mesh.nodesVector.nodes[j].rho / EPSILON_0) * h * h +
								mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].leftNodeID - 1].phi +
								mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].rightNodeID - 1].phi +
								mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].topNodeID - 1].phi +
								mesh.nodesVector.nodes[mesh.nodesVector.nodes[j].bottomNodeID - 1].phi -
								4 * mesh.nodesVector.nodes[j].phi;

							residualSum += residual * residual;
						}
					}

					if (sqrt(residualSum / static_cast<double>(mesh.numNodes)) < parametersList.residualTolerance)
					{
						parametersList.logBrief("Solver convergence criteria met", 1);
						break;
					}
				}

				// Account for periodic BCs 
				for (int i = 0; i < mesh.numNodes; i++)
				{
					if (parametersList.leftBCType == "periodic")
					{
						if (mesh.nodesVector.nodes[i].boundaryType == "TL" ||
							mesh.nodesVector.nodes[i].boundaryType == "L" ||
							mesh.nodesVector.nodes[i].boundaryType == "BL")
						{
							mesh.nodesVector.nodes[i].phi = 0.5 * (mesh.nodesVector.nodes[i].phi +
								mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX1NodeID - 1].phi);
							mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX1NodeID - 1].phi =
								mesh.nodesVector.nodes[i].phi;
						}
					}
					if (parametersList.topBCType == "periodic")
					{
						if (mesh.nodesVector.nodes[i].boundaryType == "TL" ||
							mesh.nodesVector.nodes[i].boundaryType == "T" ||
							mesh.nodesVector.nodes[i].boundaryType == "TR")
						{
							mesh.nodesVector.nodes[i].phi = 0.5 * (mesh.nodesVector.nodes[i].phi +
								mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX2NodeID - 1].phi);
							mesh.nodesVector.nodes[mesh.nodesVector.nodes[i].periodicX2NodeID - 1].phi =
								mesh.nodesVector.nodes[i].phi;
						}
					}
				}
			}
		}
	}

	// Electric field
	for (int i = 0; i < mesh.numNodes; i++)
	{
		int leftNodeID = mesh.nodesVector.nodes[i].leftNodeID - 1;
		int rightNodeID = mesh.nodesVector.nodes[i].rightNodeID - 1;
		int topNodeID = mesh.nodesVector.nodes[i].topNodeID - 1;
		int bottomNodeID = mesh.nodesVector.nodes[i].bottomNodeID - 1;
		int periodicX1NodeID = mesh.nodesVector.nodes[i].periodicX1NodeID - 1;
		int periodicX2NodeID = mesh.nodesVector.nodes[i].periodicX2NodeID - 1;

		if (mesh.nodesVector.nodes[i].boundaryType == "internal")
		{
			mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[leftNodeID].phi -
				mesh.nodesVector.nodes[rightNodeID].phi) / (2 * h);

			mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[bottomNodeID].phi -
				mesh.nodesVector.nodes[topNodeID].phi) / (2 * h);
		}
		else if (mesh.nodesVector.nodes[i].boundaryType == "T")
		{
			mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[leftNodeID].phi -
				mesh.nodesVector.nodes[rightNodeID].phi) / (2 * h);

			if (parametersList.topBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[bottomNodeID].phi -
					mesh.nodesVector.nodes[periodicX2NodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[bottomNodeID].phi) / h;
			}
		}
		else if (mesh.nodesVector.nodes[i].boundaryType == "B")
		{
			mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[leftNodeID].phi -
				mesh.nodesVector.nodes[rightNodeID].phi) / (2 * h);

			if (parametersList.bottomBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[periodicX2NodeID].phi -
					mesh.nodesVector.nodes[topNodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[topNodeID].phi) / h;
			}
		}
		else if (mesh.nodesVector.nodes[i].boundaryType == "L")
		{
			if (parametersList.leftBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[periodicX1NodeID].phi -
					mesh.nodesVector.nodes[rightNodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[rightNodeID].phi) / h;
			}

			mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[bottomNodeID].phi -
				mesh.nodesVector.nodes[topNodeID].phi) / (2 * h);
		}
		else if (mesh.nodesVector.nodes[i].boundaryType == "R")
		{
			if (parametersList.rightBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[leftNodeID].phi -
					mesh.nodesVector.nodes[periodicX1NodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[leftNodeID].phi) / h;
			}

			mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[bottomNodeID].phi -
				mesh.nodesVector.nodes[topNodeID].phi) / (2 * h);
		}
		else if (mesh.nodesVector.nodes[i].boundaryType == "TL")
		{
			if (parametersList.leftBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[periodicX1NodeID].phi -
					mesh.nodesVector.nodes[rightNodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[rightNodeID].phi) / h;
			}
			
			if (parametersList.topBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[bottomNodeID].phi -
					mesh.nodesVector.nodes[periodicX2NodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[bottomNodeID].phi) / h;
			}
		}
		else if (mesh.nodesVector.nodes[i].boundaryType == "BL")
		{
			if (parametersList.leftBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[periodicX1NodeID].phi -
					mesh.nodesVector.nodes[rightNodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[rightNodeID].phi) / h;
			}
			
			if (parametersList.topBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[periodicX2NodeID].phi -
					mesh.nodesVector.nodes[topNodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[topNodeID].phi) / h;
			}
		}
		else if (mesh.nodesVector.nodes[i].boundaryType == "TR")
		{
			if (parametersList.leftBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[leftNodeID].phi -
					mesh.nodesVector.nodes[periodicX1NodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[leftNodeID].phi) / h;
			}

			if (parametersList.topBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[bottomNodeID].phi -
					mesh.nodesVector.nodes[periodicX2NodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[bottomNodeID].phi) / h;
			}
		}
		else if (mesh.nodesVector.nodes[i].boundaryType == "BR")
		{
			if (parametersList.leftBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[leftNodeID].phi -
					mesh.nodesVector.nodes[periodicX1NodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[0] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[leftNodeID].phi) / h;
			}

			if (parametersList.topBCType == "periodic")
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[periodicX2NodeID].phi -
					mesh.nodesVector.nodes[topNodeID].phi) / (2 * h);
			}
			else
			{
				mesh.nodesVector.nodes[i].EMfield[1] = (mesh.nodesVector.nodes[i].phi -
					mesh.nodesVector.nodes[topNodeID].phi) / h;
			}
		}
	}

	// Magnetic field
	for (int i = 0; i < mesh.numNodes; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mesh.nodesVector.nodes[i].EMfield[j+3] = parametersList.Bfield[j];
		}
	}

	parametersList.logBrief("Field solver exited", 1);
}