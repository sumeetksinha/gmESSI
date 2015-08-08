/********************************************************************************************************
*  File:        Node.cpp           	        		    | Copyright:: ##############################    *
*  Description: Contains the magic of translation       | BOX 1505                                      *
*  Rev:         Version 1                               | 125 25 ALVSJO                                 *
*  Created:     June 28, 2015                           | SWEDEN                                        *
*  Author:      Sumeet Kumar Sinha                      | tel int 530-601-8271                          *
*  Email:       sumeet.kumar507@gmail.com               |                                               *
*                                                                                                       *
*  The copyright to the computer program(s) herein is the property of #######################, ######.  *
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha      *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which      *
*  the program(s) have been supplied.                                                                   *
********************************************************************************************************/

#include "Node.h"
#include <exception>
#include <iostream>

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

Node::Node(const string GmshNodeRep){

	this->setNode(GmshNodeRep);
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

void Node::setNode(const int& id,const double& X_cord, const double& Y_cord, const double& Z_cord){

	this->Id = id;
	this->Xcord = X_cord;
	this->Ycord = Y_cord;
	this->Zcord = Z_cord;
}

void Node::setNode(const string GmshNodeRep){

	try{

		Tokenizer str = Tokenizer(GmshNodeRep," ");
		this->Id = stoi(str.nextToken()); 
		this->Xcord = stod(str.nextToken()); 
		this->Ycord = stod(str.nextToken()); 
		this->Zcord = stod(str.nextToken()); 
		
	} catch(exception& e) { throw "\033[1;31mERROR:: Gmsh File has invalid symbols in Node Section. Unable to convert string to integer in Gmsh File\033[0m\n" ;}
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
