/*****************************************************************************
* 								Node.cpp
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

#include "Node.h"

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

Node::Node(){}

Node::Node(const int& id, const double& X_cord, const double& Y_cord, const double& Z_cord ){

	this->Id = id;
	this->Xcord = X_cord;
	this->Ycord = Y_cord;
	this->Zcord = Z_cord;
}

Node::~Node(){}

/******************************************************************************
****************************** Public Function*********************************
******************************************************************************/

	void Node::setId(const int& id){

		this->Id=id;
	}

	void Node::setXcord(const double& X_cord){

		this->Xcord = X_cord;
	}

	void Node::setYcord(const double& Y_cord){

		this->Ycord = Y_cord;
	}

	void Node::setZcord(const double& Z_cord){

		this->Zcord = Z_cord;
	}

	void Node::setCord(const double& X_cord, const double& Y_cord, const double& Z_cord){

		this->Xcord = X_cord;
		this->Ycord = Y_cord;
		this->Zcord = Z_cord;
	}

	int Node::getId(){

		return this->Id;
	}

	double Node::getXcord(){

		return this->Xcord;
	}

	double Node::getYcord(){

		return this->Ycord;
	}

	double Node::getZcord(){

		return this->Zcord;
	}
