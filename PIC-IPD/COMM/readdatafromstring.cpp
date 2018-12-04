/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * readdatafromstring.cpp
 *
 *  Created on: 18 Mar 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#include "readdatafromstring.hpp"

namespace Common {


void read_data_from_string::remove_space_front(std::string& line)
{
	long i, num;

	num = line.size();
	for (i = 0; i <= num-1; i++)
	{
		if(line.compare(0, 1," ") == 0) line.erase(0, 1);
	}
}


void read_data_from_string::remove_space_end(std::string& line)
{
	long i, num, num_temp;

	num = line.size();
	for (i = 0; i <= num-1; i++)
	{
		num_temp = line.size();
		if(line.compare(num_temp-1, 1," ") == 0) line.erase(num_temp-1, 1);
	}
}

void read_data_from_string::remove_comments(std::string& line)
{
	int i;
	long size1, size2;
	long index_s, index_size;
	std::string comment_s = "//";

	size1 = line.size();
	size2 = comment_s.size();


	index_s = -1;
	index_size = 0;
	for (i = 0; i <= size1-1; i++)
	{
		if (line.compare(i, size2, comment_s) == 0)
		{
			index_s = i;
		}
	}

	if (index_s != -1)
	{
		index_size = size1 - index_s;
		line.erase(index_s, index_size);
	}
}

    
    
void read_data_from_string::remove_comments(std::string& line, const std::string& comment_s)
{
    int i;
    long size1, size2;
    long index_s, index_size;
    size1 = line.size();
    size2 = comment_s.size();
    
    
    index_s = -1;
    index_size = 0;
    for (i = 0; i <= size1-1; i++)
    {
        if (line.compare(i, size2, comment_s) == 0)
        {
            index_s = i;
        }
    }
        
    if (index_s != -1)
    {
        index_size = size1 - index_s;
        line.erase(index_s, index_size);
    }
}



std::string read_data_from_string::read_string(const std::string& data, std::string read_format)
{
    std::string line = data;
	long num;
	std::string result;

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
		result = line;
	}
	else
	{
		result = "";
	}

	return (result);
}

} /* namespace Common */
