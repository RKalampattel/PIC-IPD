/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * VectorCompare.hpp
 *
 *  Created on: 21 Apr 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#ifndef COMM_VECTORCOMPARE_HPP_
#define COMM_VECTORCOMPARE_HPP_


#include <vector>

#include "common.hpp"
#include "noninstantiable.hpp"



namespace Common {

class VectorCompare: public NonInstantiable<VectorCompare> {

public:

	/*
	 * Check whether the given INT vector has same component
	 */
	static bool hasSameComponentINT(const std::vector<int>& data);
	static bool hasSameComponentINT(const std::vector<int>& data, int m);

    template <class T>
    static bool hasSameComponent(const std::vector<T>& data)
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

    template <class T>
    static bool hasSameComponent(const std::vector<T>& data, int m)
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
    
    template <class T>
    static bool hasSameComponent(const std::vector<T>& data, int low, int top)
    {
        for (int i1 = low; i1 < top ; i1++)
        {
            for (int i2 = i1+1; i2 < top; i2++)
            {
                if (data[i1] == data[i2])
                {
                    return (true);
                }
            }
        }
        
        return (false);
    }
    
    
    
    
	/*
	 * Check the range of vector
	 */
	static bool hasSatisfyRangeINT(const std::vector<int>& data, int low, int top);
	static bool hasSatisfyRangeINT(const std::vector<int>& data, int m, int low, int top);



};




} /* namespace Common */

#endif /* COMM_VECTORCOMPARE_HPP_ */
