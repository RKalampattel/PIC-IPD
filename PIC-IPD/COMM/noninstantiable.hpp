/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 *
 * 		Copyright (c) 2015-2016 MINKWAN KIM
 *
 * @file noninstantiable.hpp
 * @author minkwan
 * @date Mar 2, 2016
 * @brief Set non-instantable class
 *
 */

#ifndef NONINSTANTIABLE_HPP_
#define NONINSTANTIABLE_HPP_


namespace Common{

template <class TYPE>
class NonInstantiable
{
private:
	NonInstantiable();
};

}


#endif /* NONINSTANTIABLE_HPP_ */
