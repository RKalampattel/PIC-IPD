/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file exception.hpp
 * @author minkwan
 * @date Mar 3, 2016
 * @brief Exception handling class
 *
 */

#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include <exception>

#include "codelocation.hpp"
#include "noncopyable.hpp"


namespace Common {


// Exception manager
class COMMON_API ExceptionManager : public Common::NonCopyable<ExceptionManager>
{
public:
	ExceptionManager();
	static ExceptionManager& getInstance ();

	bool	ExceptionOutputs;
	bool	ExceptionDumps;
	bool	ExceptionAborts;
};


    
    
    
////////////////////////////////////////
// Exception and Error Handler [Basic]
////////////////////////////////////////
class COMMON_API Exception: public std::exception
{
public:
	virtual ~Exception () throw();

	void append (const std::string& add) throw ();			// Append additional description into m_what
	const std::string& str () const throw ();				// Get contents of description (m_what)
	const char* what () const throw (); 					// Get contents of description (m_what) [char]
	std::string full_description ()	const throw ();			// Show full description of exception

	virtual std::string getClassName () throw ()
	{
		return m_class_name;
	}


protected:
	CodeLocation	m_where;
	std::string		m_msg;
	std::string		m_class_name;
	std::string		m_what;
	Exception (CodeLocation where, std::string msg, std::string className) throw ();
};

    
    
// Exception and Error Description
class COMMON_API ExceptionError : public Common::Exception
{
public:
	ExceptionError (const Common::CodeLocation& where, const std::string& what, const std::string& type)
		: Exception(where, what, type)
	{

	}


	ExceptionError (const ExceptionError& e) throw()
		: Exception(e)
	{

	}
};


} /* namespace Common */

#endif /* EXCEPTION_HPP_ */
