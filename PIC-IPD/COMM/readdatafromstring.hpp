/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * readdatafromstring.hpp
 *
 *  Created on: 18 Mar 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#ifndef COMM_READDATAFROMSTRING_HPP_
#define COMM_READDATAFROMSTRING_HPP_

#include <string>
#include <sstream>
#include "noninstantiable.hpp"

namespace Common {

class read_data_from_string: public NonInstantiable<read_data_from_string>{
public:
	static void remove_space_front(std::string& line);
	static void remove_space_end(std::string& line);
	static void remove_comments(std::string& line);
    static void remove_comments(std::string& line, const std::string& comment_s);


	static std::string read_string(const std::string& data, std::string read_format);

	template<class T>
	static T read_numeric(const std::string& data, std::string read_format)
	{
        std::string line = data;
		long num;
		T result;

		// REMOVE EMPTY SPACE AT FRONT AND END
		remove_comments(line);
		remove_space_front(line);
		remove_space_end(line);

		// REMOVE EMPTY SPACE AT FRONT AND END
		remove_space_front(read_format);
		remove_space_end(read_format);


		num = read_format.size();
		if(line.compare(0, num, read_format) == 0)
		{
			line.erase(0, num);
			remove_space_front(line);
            
            std::istringstream(line) >> result;

            if (line == "") result = -1;
		}
		else
		{
			result = -1;
		}

		return (result);
	}


	template<class T>
	static T read_numeric(std::string& line, std::string read_format, T default_value)
	{
		long num;
		T result;

		// REMOVE EMPTY SPACE AT FRONT AND END
		remove_comments(line);
		remove_space_front(line);
		remove_space_end(line);

		// REMOVE EMPTY SPACE AT FRONT AND END
		remove_space_front(read_format);
		remove_space_end(read_format);


		num = read_format.size();
		if(line.compare(0, num, read_format) == 0)
		{
			line.erase(0, num);
			remove_space_front(line);
			std::istringstream(line) >> result;

			if (line == "") result = default_value;
		}
		else
		{
			result = -1;
		}

		return (result);
	}




};

} /* namespace Common */

#endif /* COMM_READDATAFROMSTRING_HPP_ */
