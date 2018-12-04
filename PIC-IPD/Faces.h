//! \file
//! \brief Definition of Faces class 
//! \author Rahul Kalampattel
//! \date Last updated November 2017

#pragma once

#include "GRID\face.hpp"

//! \class Faces
//! \brief Contains face properties, derived from FaceBase
class Faces : public FaceBase
{
public:
	// Data members


	// Constructor/destructor
	Faces();					//!< Default constructor
	Faces(FaceBase baseFace);	//!< Constructor
	~Faces();					//!< Destructor


	// Methods

};