/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file codelocation.hpp
 * @author minkwan
 * @date Mar 2, 2016
 * @brief This class stores the information about a location in the source code
 *
 */

#ifndef CODELOCATION_HPP_
#define CODELOCATION_HPP_

#include "common.hpp"


namespace Common {

class COMMON_API CodeLocation
{
public:
	explicit CodeLocation(const char *file, int line, const char *function);	// Prevent un-intended function call
	std::string str () const;

private:
	  std::string	m_file;		// File name
	  std::string	m_function;	// Function name
	  int			m_line;		// line number
};

#define FromHere() Common::CodeLocation( __FILE__ , __LINE__ , __FUNCTION__ )



} /* namespace Common */

#endif /* CODELOCATION_HPP_ */
