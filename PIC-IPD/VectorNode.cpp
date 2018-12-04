//! \file
//! \brief Implementation of VectorNode class 
//! \author Rahul Kalampattel
//! \date Last updated April 2018

#include "VectorNode.h"

// Default constructor
VectorNode::VectorNode()
{
}


// Destructor
VectorNode::~VectorNode()
{
}


// Allocate NodeBase elements to nodes data member
void VectorNode::allocate(std::vector<NodeBase> nodes)
{
	for (int i = 0; i < nodes.size(); i++)
	{
		Nodes tempNode(nodes[i]);
		this->nodes.push_back(tempNode);
	}
}


// Set charge to 0 for all elements
void VectorNode::clearChargeAndCurrent()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i].charge = 0;
		nodes[i].current = { 0.0,0.0 };
	}
}


// Set phi to 0 for all elements
void VectorNode::clearPhi()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i].phi = 0;
	}
}


// Clear fields members of nodes
void VectorNode::clearFields()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i].EMfield = { 0.0,0.0,0.0,0.0,0.0,0.0 };
	}
}


//!< Calculate potential energy
double VectorNode::calculateEP()
{
	double EP = 0.0;
	for (int i = 0; i < nodes.size(); i++)
	{
		// TODO: Should this use charge or rho? Does it need an 'abs'?
		EP += 0.5 * nodes[i].charge * nodes[i].phi;
	}
	return EP;
}
