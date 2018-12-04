/*
 * Open-source multi-Physics Phenomena Analyzer (OP2A) ver. 1.0
 * 
 *         Copyright (c) 2014-2016 MINKWAN KIM
 *
 * MultiVector.cpp
 *
 *  Created on: 26 Apr 2016
 *      Author: a1633125
 *
 *	@brief Brief description of the file
 */

#include "MultiVector.hpp"

namespace Common {


void MultiVector::resize2D(std::vector< std::vector<double> >& vec2D, int a, int b)
{
	vec2D.resize(a);
	for (int i = 0; i < a; i++) vec2D[i].resize(b);
}

void MultiVector::resize3D(std::vector< std::vector< std::vector<double> > > & vec3D, int a, int b, int c)
{
	vec3D.resize(a);
	for (int i = 0; i < a; i++)
	{
		vec3D[i].resize(b);
		for (int j = 0; j < b; j++)	vec3D[i][j].resize(c);
	}
}


std::vector< std::vector<double> > MultiVector::create2D(int a, int b)
{
	std::vector< std::vector<double> > vec2D(a, std::vector<double>(b));
	return (vec2D);
}


std::vector< std::vector< std::vector<double> > > MultiVector:: create3D(int a, int b, int c)
{
	std::vector< std::vector< std::vector<double> > > vec3D(a, std::vector< std::vector<double> >(b, std::vector<double>(c)));
	return (vec3D);
}




} /* namespace Common */
