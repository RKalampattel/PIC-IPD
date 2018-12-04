/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * fileHandle.cpp
 *
 *  Created on: 17 Jun 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#include "fileHandle.hpp"

using namespace std;

namespace Common {



void fileHandle::merge (const std::string& file1, const std::string& file2, const std::string& file3)
{
	// Create output file
	std::ofstream outFile(file3.c_str(), ios::out);

	// Open input file 1, if can't be opened, exit
	std::ifstream in1(file1.c_str());
	std::string line;

	if(!in1)
	{
		cerr << "[File Merge]: Open Failure - Cannot file 1st file" << endl;
		exit(1);
	}
	else
	{
		while(std::getline(in1, line))
		{
			outFile << line << "\n";
		}
	}
	in1.close();

	//Open input file 2, if can't be opened, exit
	ifstream in2(file2.c_str());
	if(!in2)
	{
		cerr << "[File Merge]: Open Failure - Cannot file 2nd file" << endl;
		exit(1);
	}
	else
	{
		while(std::getline(in2, line))
		{
			outFile << line << "\n";
		} //end while
	} //end else
	in2.close();

	outFile.close();
}


void fileHandle::merge (const std::string& file1, const std::string& file2, const std::string& file3, const std::string& file4)
{
	// Create output file
	std::ofstream outFile(file4.c_str(), ios::out);

	// Open input file 1, if can't be opened, exit
	std::ifstream in1(file1.c_str());
	std::string line;

	if(!in1)
	{
		cerr << "[File Merge]: Open Failure - Cannot file 1st file" << endl;
		exit(1);
	}
	else
	{
		while(std::getline(in1, line))
		{
			outFile << line << "\n";
		}
	}
	in1.close();

	//Open input file 2, if can't be opened, exit
	ifstream in2(file2.c_str());
	if(!in2)
	{
		cerr << "[File Merge]: Open Failure - Cannot file 2nd file" << endl;
		exit(1);
	}
	else
	{
		while(std::getline(in2, line))
		{
			outFile << line << "\n";
		} //end while
	} //end else
	in2.close();

	//Open input file 3, if can't be opened, exit
	ifstream in3(file3.c_str());
	if(!in3)
	{
		cerr << "[File Merge]: Open Failure - Cannot file 3rd file" << endl;
		exit(1);
	}
	else
	{
		while(std::getline(in3, line))
		{
			outFile << line << "\n";
		} //end while
	} //end else
	in3.close();

	outFile.close();
}


void fileHandle::Xmerge (const std::string& file1, const std::string& file2, const std::string& file3)
{
	merge(file1, file2, file3);
	remove(file1.c_str());
	remove(file2.c_str());
}

void fileHandle::Xmerge (const std::string& file1, const std::string& file2, const std::string& file3, const std::string& file4)
{
	merge(file1, file2, file3, file4);

	remove(file1.c_str());
	remove(file2.c_str());
	remove(file3.c_str());
}


void fileHandle::add (const std::string& file_main, const std::string& file_sub)
{
	// Open main file as an appendix mode
	std::ofstream outFile(file_main.c_str(), ios::app);
	if(!outFile)
	{
		cerr << "[File Add]: Open Failure - Cannot find main_file" << endl;
		exit(1);
	}


	// Open sub file, if can't be opened, exit
	std::ifstream subFile(file_sub.c_str());
	std::string line;

	if(!subFile)
	{
		cerr << "[File Add]: Open Failure - Cannot find sub_file" << endl;
		exit(1);
	}
	else
	{
		while(std::getline(subFile, line))
		{
			outFile << line << "\n";
		}
	}
	subFile.close();
}

void fileHandle::Xadd (const std::string& file_main, const std::string& file_sub)
{
	add(file_main, file_sub);
	remove(file_sub.c_str());
}



} /* namespace Common */
