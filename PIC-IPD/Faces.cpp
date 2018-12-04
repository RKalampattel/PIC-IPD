//! \file
//! \brief Implementation of Faces class 
//! \author Rahul Kalampattel
//! \date Last updated November 2017

#include "Faces.h"

// Default constructor
Faces::Faces()
{
}


// Constructor
Faces::Faces(FaceBase baseFace)
{
	this->connectivity = baseFace.connectivity;
	this->geometry = baseFace.geometry;
}


// Destructor
Faces::~Faces()
{
}