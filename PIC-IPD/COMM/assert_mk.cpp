/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * assert_mk.cpp
 *
 *  Created on: 21 Mar 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */


#include "common.hpp"
#include "error_exception.hpp"
#include "error_codes.hpp"
#include "assert_mk.hpp"


using namespace std;


// Constructor
AssertionManager::AssertionManager() : DoAssertions(true), AssertionDumps(true), AssertionThrows(true)
{

}


/*
 * Class functions
 */

// [CF-01] Gets the instance of the manager
AssertionManager& AssertionManager::getInstance()
{
	static AssertionManager assertion_manager;
	return assertion_manager;
}


// [CF-03] Forward declaration of the function that implements the always present assert
void AssertionManager::do_assert ( bool condition,			// Conditions
                                   const char * cond_str,	// Condition name
                                   const char * file,		// File name
                                   int line,				// Location
                                   const char * func,		// Function name
                                   const char * desc)		// Description
{
	if ((!condition) && AssertionManager::getInstance().DoAssertions)
	{
		std::ostringstream out;
		out << "Assertion failed: [" << cond_str << "] ";

		if (desc)	out << "'" << desc << "' ";
		out << "in " << file << ":" << line;

		if (func)	out << " [function " << func << "]";

		//if ( AssertionManager::getInstance().AssertionDumps )	out << "\n" << OSystem::getInstance().getProcessInfo()->getBackTrace();

		if ( AssertionManager::getInstance().AssertionThrows )
		{
			throw Common::ExceptionError(FromHere(),out.str(), Common::ErrorCodes::FailAssertion());
		}
		else
		{
			std::cerr << &out << std::endl;
			std::cerr.flush();
			abort ();
		}
	}
}


