/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * fileHandle.hpp
 *
 *  Created on: 17 Jun 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#ifndef COMM_FILEHANDLE_HPP_
#define COMM_FILEHANDLE_HPP_

#include <iostream>
#include <fstream>

#include "noninstantiable.hpp"
#include "common.hpp"


namespace Common {

class COMMON_API fileHandle: public NonInstantiable<fileHandle> {

public:
	static void merge (const std::string& file1, const std::string& file2, const std::string& file3);
	static void merge (const std::string& file1, const std::string& file2, const std::string& file3, const std::string& file4);
	static void Xmerge (const std::string& file1, const std::string& file2, const std::string& file3);
	static void Xmerge (const std::string& file1, const std::string& file2, const std::string& file3, const std::string& file4);

	static void add (const std::string& file_main, const std::string& file_sub);
	static void Xadd (const std::string& file_main, const std::string& file_sub);
};

} /* namespace Common */

#endif /* COMM_FILEHANDLE_HPP_ */
