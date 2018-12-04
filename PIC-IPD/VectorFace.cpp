//! \file
//! \brief Implementation of VectorFace class 
//! \author Rahul Kalampattel
//! \date Last updated November 2017

#include "VectorFace.h"

// Default constructor
VectorFace::VectorFace()
{
}


// Destructor
VectorFace::~VectorFace()
{
}


// Allocate FaceBase elements to faces data member
void VectorFace::allocate(std::vector<FaceBase> faces)
{
	for (int i = 0; i < faces.size(); i++)
	{
		Faces tempFace(faces[i]);
		this->faces.push_back(tempFace);
	}
}
