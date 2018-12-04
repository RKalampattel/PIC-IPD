/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file version.cpp
 * @author minkwan
 * @date Mar 10, 2016
 * @brief Class for version information
 *
 * Detailed description
 */

#include "version.hpp"

#include <iostream>


namespace Common {

version::version(int ver_main, int ver_sub, int date_year, int date_month, int date_day, const char * type_info)
:primary(ver_main), secondary(ver_sub), year(date_year), month(date_month), date(date_day), type(type_info)
{

}


void version::info(void) const
{
	std::cout << "Open-source multi-Physics Phenomena Analyzer (OP2A)	Version "<< primary << "." <<secondary << " [Type]: " << type << std::endl << std::endl;
	std::cout << "Copyright (c) 2013-" << year << "  Min Kwan Kim. All rights reserved." << std::endl << std::endl;
	std::cout << "* It is an open-source code"<< std::endl << std::endl << std::endl;
}

} /* namespace Common */
