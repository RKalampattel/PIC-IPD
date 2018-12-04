/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file version.hpp
 * @author minkwan
 * @date Mar 10, 2016
 * @brief Class for version information
 *
 * Detailed description
 */

#ifndef VERSION_HPP_
#define VERSION_HPP_

#include <string>

namespace Common {

class version {
public:
    explicit version(int ver_main, int ver_sub, int date_year, int date_month, int date_day, const char * type_info);
    void info() const;

private:
    int primary;       		/* Primary version number   (i.e. 2 for V2.0) */
    int secondary;    	 	/* Secondary version number (i.e. 0 for V2.0) */
    int year;          		/* Most recent date of update */
    int month;
    int date;

	std::string type;     	/* "Type" of version */
};



} /* namespace Common */

#endif /* VERSION_HPP_ */

