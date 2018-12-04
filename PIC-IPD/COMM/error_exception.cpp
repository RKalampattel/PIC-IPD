/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file exception.cpp
 * @author minkwan
 * @date Mar 3, 2016
 * @brief Exception handling
 *
 * Detailed description
 */

#include "error_exception.hpp"

namespace Common {


// Exception Manager
ExceptionManager::ExceptionManager()
{
    ExceptionOutputs	= true;
    ExceptionDumps		= true;
    ExceptionAborts		= false;
}

ExceptionManager& ExceptionManager::getInstance()
{
	static	ExceptionManager exception_manager;
	return 	exception_manager;
}



////////////////////////////////////////
// Exception and Error Handler [Basic]
////////////////////////////////////////
Exception::~Exception() throw ()
{

}

Exception::Exception (CodeLocation where, std::string msg, std::string className) throw () : m_where(where), m_msg(msg), m_class_name(className)
{
	m_what = full_description();

	// Exception output
	if (ExceptionManager::getInstance().ExceptionOutputs)
	{
		std::cout << "======================================" << std::endl;
		std::cout << "+++ Exception thrown +++++++++++++++++" << std::endl;
		std::cout << m_what << std::endl;
		std::cout << "======================================" << std::endl;
	}


	// Abort exception
	if (ExceptionManager::getInstance().ExceptionAborts )
	{
		std::cout << "+++ Exception aborting ... " << std::endl;
		abort();
	}


	// Dump exception
	if (ExceptionManager::getInstance().ExceptionDumps )
	{
		std::cout << "+++ Exception backtrace ++++++++++++++" << std::endl;
		std::cout << "======================================" << std::endl;
	}

#ifdef MPI
	MPI_Abort(MPI_COMM_WORLD, 0);
#endif

	abort();
}


// Append additional description into m_what
void Exception::append(const std::string& add) throw ()
{
	m_what += add;
}


// Get contents of description (m_what)
const std::string& Exception::str () const throw ()
{
	return m_what;
}


// Get contents of description (m_what) [char]
const char* Exception::what() const throw()
{
	return str().c_str();
}


// Show full description of exception
std::string Exception::full_description () const throw ()
{
	std::string desc;
	desc += "\n[Details]\n";
	desc += "+++++++++++++++++++++++++++++++++++++\n";

	desc += "From : \'";
	desc += m_where.str();
	desc += "\'\n";

	desc += "Type : \'";
	desc += m_class_name;
	desc += "\'\n";

	desc += "Message :\n";
	desc += m_msg;
	desc += "\n+++++++++++++++++++++++++++++++++++++\n";
	return desc;
}


///////////////////////////////////////////////////////////////////////////
// Overloading
std::ostream& operator<<(std::ostream& output, Common::Exception& exc)
{
	output << exc.str();
	return output;
}

std::ostream& operator<<(std::ostream& output, Common::Exception* exc)
{
	return output << *exc;
}

    
    
    
    
    
    

} /* namespace Common */
