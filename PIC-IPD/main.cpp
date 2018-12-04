//! \file
//! \brief Entry point into the simulation
//! \author Rahul Kalampattel
//! \date Last updated October 2018

#include "VectorPatch.h"

int main()
{
	// Process input parameters
	Parameters parametersList("inputs.txt");	
	parametersList.logMessages("Starting simulation", __FILENAME__, __LINE__, 1);


	if (parametersList.numErrors == 0)
		parametersList.assignInputs();

	parametersList.processMesh("PIC");

	// Commence simulation
	if (parametersList.numErrors == 0)
	{
		VectorPatch patchesVector(&parametersList);
		patchesVector.startPIC();

		parametersList.numErrors = patchesVector.numErrors;
	}


	if (parametersList.numErrors != 0)
	{
		parametersList.logMessages("Simulation complete, exited UNSUCCESSFULLY", __FILENAME__, __LINE__, 1);
		return -1;
	}
	else
	{
		parametersList.logMessages("Simulation complete, exited successfully", __FILENAME__, __LINE__, 1);
		return 0;
	}
}