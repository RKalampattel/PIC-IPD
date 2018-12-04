/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file codelocation.cpp
 * @author minkwan
 * @date Mar 2, 2016
 * @brief (Type brief description of file
 *
 * Detailed description
 */

#include "codelocation.hpp"

namespace Common {

CodeLocation::CodeLocation(const char *file, int line, const char *function)
: m_file(file), m_function(function), m_line(line)
{

}

std::string CodeLocation::str () const
{
	std::ostringstream place;
	place << m_file <<":" << m_line;
	if (!m_function.empty()) // skip if compiler doees not set function
	{
		place << ":" << m_function;
	}

	return place.str();
}

}
