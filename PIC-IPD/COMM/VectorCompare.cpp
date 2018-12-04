/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * VectorCompare.cpp
 *
 *  Created on: 21 Apr 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#include "VectorCompare.hpp"

namespace Common {





bool VectorCompare::hasSameComponentINT(const std::vector<int>& data)
{
	int m = data.size();

	for (int i1 = 0; i1 < m ; i1++)
	{
		for (int i2 = i1+1; i2 < m; i2++)
		{
			if (data[i1] == data[i2])
			{
				return (true);
			}
		}
	}

	return (false);
}



bool VectorCompare::hasSameComponentINT(const std::vector<int>& data, int m)
{
	for (int i1 = 0; i1 < m ; i1++)
	{
		for (int i2 = i1+1; i2 < m; i2++)
		{
			if (data[i1] == data[i2])
			{
				return (true);
			}
		}
	}

	return (false);
}



bool VectorCompare::hasSatisfyRangeINT(const std::vector<int>& data, int low, int top)
{
	int m = data.size();

	for (int i1 = 0; i1 < m; i1++)
	{
		if (data[i1] < low)	return (true);
		if (data[i1] > top) return (true);
	}

	return (false);
}



bool VectorCompare::hasSatisfyRangeINT(const std::vector<int>& data, int m, int low, int top)
{
	for (int i1 = 0; i1 < m; i1++)
	{
		if (data[i1] < low)	return (true);
		if (data[i1] > top) return (true);
	}

	return (false);
}



} /* namespace Common */
