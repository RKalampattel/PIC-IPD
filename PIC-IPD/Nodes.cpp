//! \file
//! \brief Implementation of Nodes class 
//! \author Rahul Kalampattel
//! \date Last updated February 2018

#include "Nodes.h"

// Default constructor
Nodes::Nodes()
{
}


// Constructor
Nodes::Nodes(NodeBase baseNode)
{
	this->connectivity = baseNode.connectivity;
	this->geometry = baseNode.geometry;
}


// Destructor
Nodes::~Nodes()
{
}