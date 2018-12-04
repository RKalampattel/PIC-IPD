/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * MultiVector.hpp
 *
 *  Created on: 26 Apr 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#ifndef COMM_MULTIVECTOR_HPP_
#define COMM_MULTIVECTOR_HPP_

#include <vector>
#include "noninstantiable.hpp"


typedef std::vector< std::vector<double> > vector2D;
typedef std::vector< std::vector< std::vector<double> > > vector3D;

typedef std::vector< std::vector<int> > intvector2D;
typedef std::vector< std::vector< std::vector<int> > > intvector3D;



namespace Common {

class MultiVector: public NonInstantiable<MultiVector>
{
public:
	static void resize2D(std::vector< std::vector<double> >& vec2D, int a, int b);
	static void resize3D(std::vector< std::vector< std::vector<double> > > & vec3D, int a, int b, int c);

	static std::vector< std::vector<double> > create2D(int a, int b);
	static std::vector< std::vector< std::vector<double> > > create3D(int a, int b, int c);
};



template <typename T>
class VectorMulti: public NonInstantiable<VectorMulti<T> >
{
public:
	static void resize(std::vector< std::vector<T> >& vec2D, int a, int b)
	{
		vec2D.resize(a);
		for (int i = 0; i < a; i++) vec2D[i].resize(b);
	}

	static void resize(std::vector< std::vector< std::vector<T> > >& vec3D, int a, int b, int c)
	{
		vec3D.resize(a);
		for (int i = 0; i < a; i++)
		{
			vec3D[i].resize(b);
			for (int j = 0; j < b; j++)	vec3D[i][j].resize(c);
		}
	}

	static std::vector< std::vector<T> > create(int a, int b)
	{
		std::vector< std::vector<T> > vec2D(a, std::vector<T>(b));
		return (vec2D);
	}

	static std::vector< std::vector< std::vector<T> > > create(int a, int b, int c)
	{
		std::vector< std::vector< std::vector<T> > > vec3D(a, std::vector< std::vector<T> >(b, std::vector<T>(c)));
		return (vec3D);
	}
};


} /* namespace Common */

#endif /* COMM_MULTIVECTOR_HPP_ */
