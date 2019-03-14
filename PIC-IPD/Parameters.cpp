//! \file
//! \brief Implementation of Parameters class 
//! \author Rahul Kalampattel
//! \date Last updated March 2019

#include "Parameters.h"

// Default constructor
Parameters::Parameters()
{
}


// Constructor
Parameters::Parameters(std::string filename)
{
	initialTime = std::chrono::system_clock::now();
	std::ifstream inputFile(filename);	// Open input file
	
	char firstCharacter;
	std::string name, value, test;

	if (inputFile.is_open())
	{
		logMessages("Reading inputs", __FILENAME__, __LINE__, 1);

		while (!inputFile.eof())	// Continue until the end of the file is reached
		{
			// Check what the first character in each line is
			firstCharacter = static_cast<char>(inputFile.get());

			// Check for commented or empty lines
			if (firstCharacter == '%' || firstCharacter == '\n')
			{
				inputFile.ignore(256, '\n');
			}
			// Store values in a string vector, names and values are space separated
			else
			{
				inputFile >> name >> value;
				if (value.front() != '%' && value.back() != ':')
				{
					valuesVector.push_back(value);
					inputFile.ignore(256, '\n');
				}
				// Check for empty arguments
				else
				{
					while (true)
					{
						valuesVector.push_back("DEFAULT");
						if (value.back() == ':')
						{
							inputFile >> value;
							if (value.front() == '%')
							{
								valuesVector.push_back("DEFAULT");
								inputFile.ignore(256, '\n');
								break;
							}
							else if (value.back() != ':')
							{
								valuesVector.push_back(value);
								inputFile.ignore(256, '\n');
								break;
							}
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		inputFile.close();
	}
	else
	{
		logMessages("Unable to open input file", __FILE__, __LINE__, 3);
		fileNotOpened = true;
	}
}


// Destructor
Parameters::~Parameters()
{
}


// Assign values to data members 
void Parameters::assignInputs()
{
	logMessages("Printing input parameters", __FILENAME__, __LINE__, 1);
	if (!fileNotOpened)
	{
		int index = 0;


		// Global simulation parameters
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			timeStep = stod(valuesVector[index]);
			if (timeStep <= 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for time step, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid type detected for time step, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Time step should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.0001";
			timeStep = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Time step: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			maximumNumberOfIterations = stoi(valuesVector[index]);
			if (maximumNumberOfIterations < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for max number of iterations, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for max number of iterations, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Max number of iterations should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "10";
			maximumNumberOfIterations = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Maximum number of iterations: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			numberOfPatches = stoi(valuesVector[index]);
			if (numberOfPatches < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for number of patches, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for number of patches, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Number of patches should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1";
			numberOfPatches = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Number of patches: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			initialParticlesPerCell = stoi(valuesVector[index]);
			if (initialParticlesPerCell < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for initial particles per cell, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for initial particles per cell, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Initial particles per cell should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1";
			initialParticlesPerCell = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Initial particles per cell: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			minimumParticlesPerCell = stoi(valuesVector[index]);
			if (minimumParticlesPerCell < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for minimum particles per cell, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for minimum particles per cell, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Minimum particles per cell should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "4";
			minimumParticlesPerCell = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Minimum particles per cell: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			maximumParticlesPerCell = stoi(valuesVector[index]);
			if (maximumParticlesPerCell <= minimumParticlesPerCell)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for maximum particles per cell, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for maximum particles per cell, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Maximum particles per cell should be greater than minimum, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			maximumParticlesPerCell = 6 + minimumParticlesPerCell;
			valuesVector[index] = std::to_string(maximumParticlesPerCell);
			useDefaultArgument = false;
		}
		logBrief("Maximum particles per cell: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			numCellsWithParticles = stoi(valuesVector[index]);
			if (numCellsWithParticles < 0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for number of cells with particles, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for number of cells with particles, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Number of cells with particles should be positive or zero, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0";
			numCellsWithParticles = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Number of cells with particles: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			simulationType = valuesVector[index];
			if (simulationType == "partial" || simulationType == "electron")
			{
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for simulation type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for simulation type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Simulation type should be partial or electron, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "electron";
			simulationType = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Simulation type: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			int value = stoi(valuesVector[index]);
			if (value == 1)
			{
				axisymmetric = true;
			}
			else if (value == 0)
			{
				axisymmetric = false;
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for axisymmetric flag, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for axisymmetric flag, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Axisymmetric flag should be true (1) or false (0), default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0";
			axisymmetric = false;
			useDefaultArgument = false;
		}
		logBrief("Axisymmetric flag: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			int value = stoi(valuesVector[index]);
			if (value == 1)
			{
				twoStream = true;
			}
			else if (value == 0)
			{
				twoStream = false;
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for two-stream flag, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for two-stream flag, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Two-stream flag should be true (1) or false (0), default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0";
			twoStream = false;
			useDefaultArgument = false;
		}
		logBrief("Two-stream flag: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			int value = stoi(valuesVector[index]);
			if (value == 1)
			{
				inletSource = true;
			}
			else if (value == 0)
			{
				inletSource = false;
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for inlet source flag, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for inlet source flag, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Inlet source flag should be true (1) or false (0), default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0";
			inletSource = false;
			useDefaultArgument = false;
		}
		logBrief("Inlet source: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			inletSizePercent = stod(valuesVector[index]);
			if (inletSizePercent < 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for inlet size, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid type detected for inlet size, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Inlet size should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "10.0";
			inletSizePercent = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Inlet size: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			inletFlowRate = stod(valuesVector[index]);
			if (inletFlowRate < 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for inlet flow rate, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid type detected for inlet flow rate, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Inlet flow rate should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1.0";
			inletFlowRate = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Inlet flow rate: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			inletVelocity = stod(valuesVector[index]);
			if (inletVelocity < 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for inlet velocity, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid type detected for inlet velocity, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Inlet velocity should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1.0";
			inletVelocity = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Inlet velocity: " + valuesVector[index], 1);
		index++;


		// Particle and collision parameters
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			particleDistribution = valuesVector[index];
			if (particleDistribution == "uniform" || particleDistribution == "random" || particleDistribution == "precise")
			{
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for particle distribution, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for particle distribution, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Particle distribution should be uniform, random or precise, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "random";
			particleDistribution = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Particle distribution: " + valuesVector[index], 1);
		index++;


		// TODO: Test independence of specific weight parameter
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			specificWeight = stod(valuesVector[index]);
			if (specificWeight < 1.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for specific weight, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid type detected for specific weight, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Specific weight should be greater than 1.0, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1.0";
			specificWeight = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Specific weight: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			initialTemperature = stod(valuesVector[index]);
			if (initialTemperature < 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for initial temperature, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid type detected for initial temperature, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Initial temperature should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1000.0";
			initialTemperature = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Initial temperature: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}

			std::stringstream inputs(valuesVector[index]);
			std::vector<std::string> outputs;

			while (inputs.good())
			{
				std::string value;
				std::getline(inputs, value, ',');
				outputs.push_back(value);
			}

			for (int i = 0; i < outputs.size(); i++)
			{
				initialPosition.push_back(stod(outputs[i]));
				if (initialPosition[i] < 0.0 || initialPosition[i] > 1.0)
				{
					throw 1;
				}
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for initial position, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for initial position, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Initial position should be between 0 and 1, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.5,0.5";
			initialPosition = { 0.5, 0.5 };
			useDefaultArgument = false;
		}
		logBrief("Initial position: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			
			std::stringstream inputs(valuesVector[index]);
			std::vector<std::string> outputs;

			while (inputs.good())
			{
				std::string value;
				std::getline(inputs, value, ',');
				outputs.push_back(value);
			}

			for (int i = 0; i < outputs.size(); i++)
			{
				initialVelocity.push_back(stod(outputs[i]));
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for initial velocity, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for initial velocity, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.0,0.0";
			initialVelocity = { 0.0, 0.0 };
			useDefaultArgument = false;
		}
		logBrief("Initial velocity: " + valuesVector[index], 1);
		index++;

		
		// TODO: Add more propellants
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			propellant = valuesVector[index];
			if (propellant != "xenon")
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for propellant, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for propellant, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Invalid propellant selected, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "xenon";
			propellant = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Propellant: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			MCCfrequency = stoi(valuesVector[index]);
			if (MCCfrequency < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for MCC frequency, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for MCC frequency, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("MCC frequency should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "11";
			MCCfrequency = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("MCC frequency: " + valuesVector[index], 1);
		index++;


		// Field and FDTD parameters
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}

			std::stringstream inputs(valuesVector[index]);
			std::vector<std::string> outputs;

			while (inputs.good())
			{
				std::string value;
				std::getline(inputs, value, ',');
				outputs.push_back(value);
			}

			for (int i = 0; i < outputs.size(); i++)
			{
				Efield.push_back(stod(outputs[i]));
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for electric field, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for electric field, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.0,0.0,0.0";
			Efield = { 0.0,0.0,0.0 };
			useDefaultArgument = false;
		}
		logBrief("Electric field: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			
			std::stringstream inputs(valuesVector[index]);
			std::vector<std::string> outputs;

			while (inputs.good())
			{
				std::string value;
				std::getline(inputs, value, ',');
				outputs.push_back(value);
			}

			for (int i = 0; i < outputs.size(); i++)
			{
				Bfield.push_back(stod(outputs[i]));
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for magnetic field, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for magnetic field, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.0,0.0,0.0";
			Bfield = { 0.0,0.0,0.0 };
			useDefaultArgument = false;
		}
		logBrief("Magnetic field: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			FDTDiterations = stoi(valuesVector[index]);
			if (FDTDiterations < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for FDTD iterations, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for FDTD iterations, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("FDTD iterations should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1000000";
			FDTDiterations = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("FDTD iterations: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			FDTDfrequency = stoi(valuesVector[index]);
			if (FDTDfrequency < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for FDTD frequency, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for FDTD frequency, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("FDTD frequency should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "11";
			FDTDfrequency = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("FDTD frequency: " + valuesVector[index], 1);
		index++;


		// Mesh and domain parameters
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			int value = stoi(valuesVector[index]);
			if (value == 1)
			{
				userMesh = true;
			}
			else if (value == 0)
			{
				userMesh = false;
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for user defined mesh flag, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for user defined mesh flag, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("User defined mesh flag should be true (1) or false (0), default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1";
			userMesh = true;
			useDefaultArgument = false;
		}
		logBrief("User defined mesh flag: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			domainLength = stod(valuesVector[index]);
			if (domainLength <= 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for domain length, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (const std::exception&)
		{
			logBrief("Invalid argument detected for domain length, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Domain length should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.1";
			domainLength = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Domain length: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			domainHeight = stod(valuesVector[index]);
			if (domainHeight <= 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for domain height, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (const std::exception&)
		{
			logBrief("Invalid argument detected for domain height, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Domain height should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.06";
			domainHeight = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Domain height: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			PICspacing = stod(valuesVector[index]);
			if (PICspacing <= 0.0 || PICspacing >= domainLength || PICspacing >= domainHeight)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for PIC grid spacing, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (const std::exception&)
		{
			logBrief("Invalid argument detected for PIC grid spacing, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("PIC grid spacing should be positive and less than domain size, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			if (domainLength > domainHeight)
			{
				PICspacing = domainLength / 5.0;
			}
			else
			{
				PICspacing = domainHeight/ 5.0;
			}
			valuesVector[index] = std::to_string(PICspacing);
			useDefaultArgument = false;
		}
		logBrief("PIC grid spacing: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			FDTDspacing = stod(valuesVector[index]);
			if (FDTDspacing <= 0.0 || FDTDspacing > PICspacing)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for FDTD grid spacing, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (const std::exception&)
		{
			logBrief("Invalid argument detected for FDTD grid spacing, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("FDTD grid spacing should be positive and less than PIC spacing, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			FDTDspacing = PICspacing / 10.0;
			valuesVector[index] = std::to_string(FDTDspacing);
			useDefaultArgument = false;
		}
		logBrief("FDTD grid spacing: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			meshFilePath = valuesVector[index];
		}
		catch (double error)
		{
			logBrief("No argument detected for mesh file path, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for mesh file path, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "coarseMesh.su2";
			meshFilePath = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Mesh file path: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			meshScalingParameter = stod(valuesVector[index]);
			if (meshScalingParameter <= 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for mesh scaling parameter, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (const std::exception&)
		{
			logBrief("Invalid argument detected for mesh scaling parameter, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Mesh scaling parameter should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1.0";
			meshScalingParameter = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Mesh scaling parameter: " + valuesVector[index], 1);
		index++;


		// Solver and boundary condition parameters
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			solverType = valuesVector[index];
			if (solverType == "GS" || solverType == "FFT")
			{
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for solver type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (const std::exception&)
		{
			logBrief("Invalid argument detected for solver type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Solver type should be GS or FFT, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "GS";
			solverType = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Solver type: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			maxSolverIterations = stoi(valuesVector[index]);
			if (maxSolverIterations < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for max solver iterations, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for max solver iterations, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Max solver iterations should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "50";
			maxSolverIterations = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Maximum solver iterations: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			residualTolerance = stod(valuesVector[index]);
			if (residualTolerance <= 0.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for residual tolerance, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for residual tolerance, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Residual tolerance should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1e-20";
			residualTolerance = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Residual tolerance: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			SORparameter = stod(valuesVector[index]);
			if (SORparameter < 1.0)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for SOR parameter, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for SOR parameter, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("SOR parameter should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1.1";
			SORparameter = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Successive over-relaxation parameter: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			leftBCType = valuesVector[index];
			if (leftBCType == "periodic" || leftBCType == "open" || leftBCType == "dirichlet" || leftBCType == "neumann")
			{
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for left BC type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for left BC type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Left BC type should be periodic, open, dirichlet or neumann, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "dirichlet";
			leftBCType = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Left boundary condition type: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			leftBCValue = stod(valuesVector[index]);
		}
		catch (double error)
		{
			logBrief("No argument detected for left BC value, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for left BC value, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.0";
			leftBCValue = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Left boundary condition value: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			rightBCType = valuesVector[index];
			if (rightBCType == "periodic" || rightBCType == "open" || rightBCType == "dirichlet" || rightBCType == "neumann")
			{
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for right BC type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for right BC type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Right BC type should be periodic, open, dirichlet or neumann, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "dirichlet";
			rightBCType = valuesVector[index];
			useDefaultArgument = false;
		}
		if ((rightBCType == "periodic" && leftBCType != "periodic") || (rightBCType != "periodic" && leftBCType == "periodic"))
		{
			logBrief("Periodic boundary conditions must be used on left and right boundaries", 3);
		}
		logBrief("Right boundary condition type: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			rightBCValue = stod(valuesVector[index]);
		}
		catch (double error)
		{
			logBrief("No argument detected for right BC value, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for right BC value, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.0";
			rightBCValue = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Right boundary condition value: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			topBCType = valuesVector[index];
			if (topBCType == "periodic" || topBCType == "open" || topBCType == "dirichlet" || topBCType == "neumann")
			{
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for top BC type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for top BC type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Top BC type should be periodic, open, dirichlet or neumann, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "dirichlet";
			topBCType = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Top boundary condition type: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			topBCValue = stod(valuesVector[index]);
		}
		catch (double error)
		{
			logBrief("No argument detected for top BC value, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for top BC value, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.0";
			topBCValue = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Top boundary condition value: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			bottomBCType = valuesVector[index];
			if (bottomBCType == "periodic" || bottomBCType == "open" || bottomBCType == "dirichlet" || bottomBCType == "neumann")
			{
			}
			else
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for bottom BC type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for bottom BC type, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Bottom BC type should be periodic, open, dirichlet or neumann", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "dirichlet";
			bottomBCType = valuesVector[index];
			useDefaultArgument = false;
		}
		if ((bottomBCType == "periodic" && topBCType != "periodic") || (bottomBCType != "periodic" && topBCType == "periodic"))
		{
			logBrief("Periodic boundary conditions must be used on top and bottom boundaries", 3);
		}
		if (bottomBCType == "periodic" && axisymmetric == true)
		{
			logBrief("Cannot have periodic top/bottom BCs in axisymmetric simulations", 3);
		}
		if (bottomBCType == "periodic" && rightBCType == "periodic")
		{
			logBrief("Cannot have periodic BCs in all four directions", 3);
		}
		logBrief("Bottom boundary condition type: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			bottomBCValue = stod(valuesVector[index]);
		}
		catch (double error)
		{
			logBrief("No argument detected for bottom BC value, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for bottom BC value, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "0.0";
			bottomBCValue = stod(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Bottom boundary condition value: " + valuesVector[index], 1);
		index++;


		// Parallelisation parameters
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			numThreads = stoi(valuesVector[index]);
			if (numThreads < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for number of OpenMP threads, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for number of OpenMP threads, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Number of OpenMP threads should be greater than 0, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "1";
			numThreads = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Number of OpenMP threads: " + valuesVector[index], 1);
		index++;


		// Output parameters
		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			plotFrequency = stoi(valuesVector[index]);
			if (plotFrequency < 1)
			{
				throw 1;
			}
		}
		catch (double error)
		{
			logBrief("No argument detected for plotting frequency, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for plotting frequency, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (int error)
		{
			logBrief("Plotting frequency should be positive, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "11";
			plotFrequency = stoi(valuesVector[index]);
			useDefaultArgument = false;
		}
		logBrief("Plotting frequency: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			tecplotMesh = valuesVector[index];
		}
		catch (double error)
		{
			logBrief("No argument detected for output mesh file name, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for output mesh file name, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "cMesh";
			tecplotMesh = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Output mesh file name: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			tecplotParticleSolution = valuesVector[index];
		}
		catch (double error)
		{
			logBrief("No argument detected for particle solution file name, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for particle solution file name, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "cSolution_P";
			tecplotParticleSolution = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Particle solution file name: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			tecplotNodeSolution = valuesVector[index];
		}
		catch (double error)
		{
			logBrief("No argument detected for node solution file name, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for node solution file name, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "cSolution_N";
			tecplotNodeSolution = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Node solution file name: " + valuesVector[index], 1);
		index++;


		try
		{
			if (valuesVector[index] == "DEFAULT")
			{
				throw 0.0;
			}
			tecplotGlobalSolution = valuesVector[index];
		}
		catch (double error)
		{
			logBrief("No argument detected for global solution file name, default value will be used", 2);
			useDefaultArgument = true;
		}
		catch (std::invalid_argument&)
		{
			logBrief("Invalid argument detected for global solution file name, default value will be used", 2);
			useDefaultArgument = true;
		}
		if (useDefaultArgument == true)
		{
			valuesVector[index] = "cSolution_G";
			tecplotGlobalSolution = valuesVector[index];
			useDefaultArgument = false;
		}
		logBrief("Global solution file name: " + valuesVector[index], 1);
		index++;
	}
}


// Process mesh file
void Parameters::generateMesh(std::string type)
{
	double h;
	std::string meshFile;
	if (type == "PIC")
	{
		h = PICspacing;
		meshFile = meshFilePIC;
	}
	else if (type == "FDTD")
	{
		// Since the effective grid spacing of a Yee mesh is half that of a regular
		// mesh, we divide the input spacing by two
		h = FDTDspacing / 2.0;
		meshFile = meshFileFDTD;
	}

	if (abs((domainLength / h) - round(domainLength / h)) <= 1e-10 &&
		abs((domainHeight / h) - round(domainHeight / h)) <= 1e-10)
	{
		// Number of cells in each direction (length and height)
		int NL = round(domainLength / h);
		int NH = round(domainHeight / h);

		// Number of cells, nodes and faces
		int NCM = NL * NH;
		int NNM = (NL + 1) * (NH + 1);
		int NFM = (NL + 1) * NH + (NH + 1) * NL;

		std::vector<std::vector<double>> nodesVector;
		std::vector<std::vector<int>> cellsVector;
		std::vector<std::vector<int>> facesVector;

		// Create nodes
		int nodeID = 0;
		for (int i = 0; i < NL + 1; i++)
		{
			for (int j = 0; j < NH + 1; j++)
			{
				std::vector<double> node = { h * i,domainHeight - h * j };
				nodesVector.push_back(node);

				// Check for boundary nodes
				if (abs(nodesVector[nodeID][0] - 0.0) <= 1e-10 ||
					abs(nodesVector[nodeID][0] - domainLength) <= 1e-10 ||
					abs(nodesVector[nodeID][1] - 0.0) <= 1e-10 ||
					abs(nodesVector[nodeID][1] - domainHeight) <= 1e-10)
				{
					nodesVector[nodeID].push_back(1.0);
				}
				else
				{
					nodesVector[nodeID].push_back(0.0);
				}

				nodeID++;
			}
		}

		// Create cells and link to nodes
		for (int cellID = 1; cellID < NCM + 1; cellID++)
		{
			int column = floor(cellID / NH);
			int row = cellID % NH;
			if (row == 0)
			{
				column--;
				row = NH;
			}

			int node1 = (NH + 1)*column + row;
			int node2 = (NH + 1)*column + row + 1;
			int node3 = (NH + 1)*(column + 1) + row + 1;
			int node4 = (NH + 1)*(column + 1) + row;

			std::vector<int> cell = { node1,node2,node3,node4,0,0,0,0 };
			cellsVector.push_back(cell);
		}

		// Create faces and link to nodes and cells
		int faceID = 0;
		for (int cellID = 1; cellID < NCM + 1; cellID++)
		{
			int node1 = cellsVector[cellID - 1][0];
			int node2 = cellsVector[cellID - 1][1];
			int node3 = cellsVector[cellID - 1][2];
			int node4 = cellsVector[cellID - 1][3];

			// Left face
			if (nodesVector[node1 - 1][2] == 1.0 &&
				nodesVector[node2 - 1][2] == 1.0)
			{
				faceID++;
				std::vector<int> face = { -1,node1,node2,cellID,0 };
				facesVector.push_back(face);
				cellsVector[cellID - 1][4] = faceID;
			}
			else if (node1 < node2)
			{
				faceID++;
				std::vector<int> face = { 0,node2,node1,cellID - NH,cellID };
				facesVector.push_back(face);
				cellsVector[cellID - 1][4] = faceID;
				cellsVector[cellID - NH - 1][6] = faceID;
			}

			// Bottom face
			if (nodesVector[node2 - 1][2] == 1.0 &&
				nodesVector[node3 - 1][2] == 1.0)
			{
				faceID++;
				std::vector<int> face = { -1,node2,node3,cellID,0 };
				facesVector.push_back(face);
				cellsVector[cellID - 1][5] = faceID;
			}
			else if (node2 < node3)
			{
				faceID++;
				std::vector<int> face = { 0,node2,node3,cellID,cellID + 1 };
				facesVector.push_back(face);
				cellsVector[cellID - 1][5] = faceID;
				cellsVector[cellID][7] = faceID;
			}

			// Right face
			if (nodesVector[node3 - 1][2] == 1.0 &&
				nodesVector[node4 - 1][2] == 1.0)
			{
				faceID++;
				std::vector<int> face = { -1,node3,node4,cellID,0 };
				facesVector.push_back(face);
				cellsVector[cellID - 1][6] = faceID;
			}

			// Top face
			if (nodesVector[node4 - 1][2] == 1.0 &&
				nodesVector[node1 - 1][2] == 1.0)
			{
				faceID++;
				std::vector<int> face = { -1,node4,node1,cellID,0 };
				facesVector.push_back(face);
				cellsVector[cellID - 1][7] = faceID;
			}
		}

		std::ofstream mesh_file_OP2(meshFile + ".op2", std::ios::trunc);

		// Write mesh file in OP2A format
		if (mesh_file_OP2.is_open())
		{
			mesh_file_OP2 << "%*****************************************************" << std::endl;
			mesh_file_OP2 << "%     OP2A Grid File using FORMAT version 1.0         " << std::endl;
			mesh_file_OP2 << "%                                                     " << std::endl;
			mesh_file_OP2 << "%                  Originally developed by Minkwan Kim" << std::endl;
			mesh_file_OP2 << "%=====================================================" << std::endl;
			mesh_file_OP2 << "% [FACE / CELL TYPE identifiers]                      " << std::endl;
			mesh_file_OP2 << "%      Line                   :  3                    " << std::endl;
			mesh_file_OP2 << "%      Triangle (3-nodes)     :  5                    " << std::endl;
			mesh_file_OP2 << "%      Quadrilateral (4-nodes):  9                    " << std::endl;
			mesh_file_OP2 << "%      Tetrahedral (4-nodes)  : 10                    " << std::endl;
			mesh_file_OP2 << "%      Hexahedral (8-nodes)   : 12                    " << std::endl;
			mesh_file_OP2 << "%      Prism (6-nodes)        : 13                    " << std::endl;
			mesh_file_OP2 << "%      Pyramid (5-nodes)      : 14                    " << std::endl;
			mesh_file_OP2 << "%                                                     " << std::endl;
			mesh_file_OP2 << "% [BC TYPE identifiers]                               " << std::endl;
			mesh_file_OP2 << "%      Interior               :  0                    " << std::endl;
			mesh_file_OP2 << "%      Wall                   : 10                    " << std::endl;
			mesh_file_OP2 << "%      Inlet                  : 20                    " << std::endl;
			mesh_file_OP2 << "%      Outlet                 : 30                    " << std::endl;
			mesh_file_OP2 << "%      Freestream (Far-field) : 40                    " << std::endl;
			mesh_file_OP2 << "%      Symmetric              : 50                    " << std::endl;
			mesh_file_OP2 << "%      Axisymmetric           : 60                    " << std::endl;
			mesh_file_OP2 << "%      Anode                  : 70                    " << std::endl;
			mesh_file_OP2 << "%      Cathode                : 80                    " << std::endl;
			mesh_file_OP2 << "%      Dielectric material    : 90                    " << std::endl;
			mesh_file_OP2 << "%                                                     " << std::endl;
			mesh_file_OP2 << "% [DATA Format]                                       " << std::endl;
			mesh_file_OP2 << "%    -Node Data FORMAT                                " << std::endl;
			mesh_file_OP2 << "%     (NODE ID#)  (POSITION DATA: X Y Z)           " << std::endl;
			mesh_file_OP2 << "%    -Face Data FORMAT                                " << std::endl;
			mesh_file_OP2 << "%     (FACE ID#)  (BOUNDARY CONDITION)  (FACE TYPE)  (LIST OF NODE IDs) (LEFT-CELL ID) (RIGHT-CELL ID)" << std::endl;
			mesh_file_OP2 << "%    -Cell Data FORMAT" << std::endl;
			mesh_file_OP2 << "%     (CELL ID#)  (BOUNDARY CONDITION)  (CELL TYPE)  (LIST OF NODE IDs) (LIST OF FACE IDs)" << std::endl;
			mesh_file_OP2 << "%=====================================================" << std::endl;
			mesh_file_OP2 << "%                   Last Format Updated on April/2016 " << std::endl;
			mesh_file_OP2 << "%                                       by M Kim      " << std::endl;
			mesh_file_OP2 << "%*****************************************************" << std::endl << std::endl;

			mesh_file_OP2 << "% OVERALL GRID INFORMATION" << std::endl;
			mesh_file_OP2 << "DIM= " << 2 << std::endl;
			mesh_file_OP2 << "NNM= " << NNM << std::endl;
			mesh_file_OP2 << "NFM= " << NFM << std::endl;
			mesh_file_OP2 << "NCM= " << NCM << std::endl;
			mesh_file_OP2 << std::endl;

			mesh_file_OP2 << "%%%%%%%%%%%" << std::endl;
			mesh_file_OP2 << "NODEDATA= " << NNM << std::endl;
			for (int n = 0; n < NNM; n++)
			{
				mesh_file_OP2 << std::setw(8) << n + 1 << std::setw(25) <<
					std::scientific << std::setprecision(16) <<
					nodesVector[n][0] << std::setw(25) << nodesVector[n][1] <<
					std::setw(25) << 0.0 << std::endl;
			}
			mesh_file_OP2 << std::endl;

			mesh_file_OP2 << "%%%%%%%%%%%%" << std::endl;
			mesh_file_OP2 << "FACEDATA= " << NFM << std::endl;
			for (int f = 0; f < NFM; f++)
			{
				mesh_file_OP2 << std::setw(8) << f + 1 << std::setw(8) <<
					facesVector[f][0] << std::setw(8) << 3;
				for (int i = 1; i <= 4; i++)
					mesh_file_OP2 << std::setw(8) << facesVector[f][i];
				mesh_file_OP2 << std::endl;
			}
			mesh_file_OP2 << std::endl;

			mesh_file_OP2 << "%%%%%%%%%%%" << std::endl;
			mesh_file_OP2 << "CELLDATA= " << NCM << std::endl;
			for (int c = 0; c < NCM; c++)
			{
				mesh_file_OP2 << std::setw(8) << c + 1 << std::setw(8) << 0 <<
					std::setw(8) << 9;
				for (int i = 0; i < 8; i++)
					mesh_file_OP2 << std::setw(8) << cellsVector[c][i];
				mesh_file_OP2 << std::endl;
			}
			mesh_file_OP2 << std::endl;
			mesh_file_OP2.close();
		}
	}
	else
	{
		logBrief("Mesh count is non-integer, adjust grid spacing", 3);
	}
}


// Process mesh file
void Parameters::processMesh(std::string type)
{
	if (type == "PIC")
	{
		if (userMesh)
		{
			logMessages("Generating user mesh", __FILENAME__, __LINE__, 1);
			generateMesh("PIC");
		}
		else
		{
			precessingGridSU2(meshFilePath, meshFilePIC);
		}
		logMessages("Extracting mesh data", __FILENAME__, __LINE__, 1);
		readGridFromFile(meshFilePIC + ".op2", gridinfoPIC, gridgeoPIC);
		processingGrid(gridinfoPIC, gridgeoPIC);
	}
	else if (type == "FDTD")
	{
		logBrief("Generating FDTD mesh and extracting data", 1);
		generateMesh("FDTD");
		readGridFromFile(meshFileFDTD + ".op2", gridinfoFDTD, gridgeoFDTD);
		processingGrid(gridinfoFDTD, gridgeoFDTD);
	}
}


// Log messages, warnings and errors
void Parameters::logMessages(std::string message, std::string filename, int line, int messageType)
{
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> duration;
	duration = currentTime - initialTime;

	if (!firstLog)
	{
		std::ofstream logFile("logFile.txt", std::ios::app);	// Open log file, 'append' write mode

		if (!logFile.is_open())
		{
			while (!logFile.is_open())
			{
				std::cout << "Unable to open log file!!!" << std::endl;
				logFile.close();
				std::ofstream logFile("logFile.txt", std::ios::app);
				if (logFile.is_open())
				{
					break;
				}
			}
		}

		if (messageType == 1)
		{
			logFile << std::left << std::setfill('.') << std::setw(45) << "(" +
				filename + ", line " + std::to_string(line) + ")" << message <<
				std::right << std::setw(100 - message.length()) << "Elapsed time: " +
				std::to_string(duration.count()) + " seconds" << std::endl;
		}
		else if (messageType == 2)
		{
			logFile << std::left << std::setfill('.') << std::setw(45) << "(" +
				filename + ", line " + std::to_string(line) + ")" << "## WARNING: " +
				message << std::right << std::setw(100 - message.length()) << 
				"Elapsed time: " + std::to_string(duration.count()) + " seconds" <<
				std::endl;
		}
		else if (messageType == 3)
		{
			logFile << std::left << std::setfill('.') << std::setw(45) << "(" +
				filename + ", line " + std::to_string(line) + ")" << "#### ERROR: " + 
				message << std::right << std::setw(100 - message.length()) << 
				"Elapsed time: " + std::to_string(duration.count()) + " seconds" <<
				std::endl;
			numErrors += 1;
		}
		logFile.close();

		std::cout << message << std::endl;
	}
	else
	{
		std::ofstream logFile("logFile.txt", std::ios::trunc);	// Open log file, 'truncate' write mode

		if (!logFile.is_open())
		{
			while (!logFile.is_open())
			{
				std::cout << "Unable to open log file!!!" << std::endl;
				logFile.close();
				std::ofstream logFile("logFile.txt", std::ios::trunc);
				if (logFile.is_open())
				{
					break;
				}
			}
		}

		char time[26];
		std::time_t clockTime = std::chrono::system_clock::to_time_t(currentTime);
		ctime_s(time, sizeof time, &clockTime);
		logFile << "Simulation start time: " << time << std::endl;
			
		logFile << std::left << std::setfill('.') << std::setw(45) << "(" + 
			filename + ", line " + std::to_string(line) + ")" << message << 
			std::right << std::setw(100 - message.length()) << "Elapsed time: " + 
			std::to_string(duration.count()) + " seconds" << std::endl;
		logFile.close();

		std::cout << message << std::endl;
		firstLog = false;
	}

}


// Log brief messages
void Parameters::logBrief(std::string message, int messageType)
{
	std::ofstream logFile("logFile.txt", std::ios::app);	// Open log file, 'append' write mode

	if (!logFile.is_open())
	{ 
		while (!logFile.is_open())
		{
			std::cout << "Unable to open log file!!!" << std::endl;
			logFile.close();
			std::ofstream logFile("logFile.txt", std::ios::app);
			if (logFile.is_open())
			{
				break;
			}
		}
	}

	if (messageType == 1)
	{
		logFile << std::left << std::setw(45) << " " << message << std::endl;
	}
	else if (messageType == 2)
	{
		logFile << std::left << std::setw(45) << " " << "## WARNING: " + message << std::endl;
	}
	else if (messageType == 3)
	{
		logFile << std::left << std::setw(45) << " " << "#### ERROR: " + message << std::endl;
		numErrors += 1;
	}
	logFile.close();

	std::cout << message << std::endl;
}