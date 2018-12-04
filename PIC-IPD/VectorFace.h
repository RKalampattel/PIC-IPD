//! \file
//! \brief Definition of VectorFace class 
//! \author Rahul Kalampattel
//! \date Last updated November 2017

#pragma once

#include "Faces.h"

//! \class VectorFace
//! \brief Create and manage a vector of Faces objects
class VectorFace
{
public:
	// Data members
	std::vector<Faces> faces;					//!< Vector of Faces objects


	// Constructor/destructor
	VectorFace();								//!< Default constructor
	~VectorFace();								//!< Destructor


	// Methods
	void allocate(std::vector<FaceBase> faces);	//!< Allocate FaceBase elements to faces data member
};