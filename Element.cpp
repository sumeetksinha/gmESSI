/********************************************************************************************************
*  File:        Element.cpp            	        	    | Copyright:: ##############################    *
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

#include "Element.h"

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

Element::Element(){}

Element::Element(const int& id, const int& type, const int& nofTag, const int& physicalTag, const int& entityTag, vector<int> nodeList){

	this->Id = id;
	this->Type = type;
	this->NofTag = nofTag;
	this->PhysicalTag = physicalTag;
	this->EntityTag = entityTag;
	this->NodeList = nodeList;
}

Element::Element(const string ElementDes){

	this->setElement(ElementDes);
}

Element::~Element(){}

/******************************************************************************
****************************** Public Function*********************************
******************************************************************************/

void Element::setElement(const string ElementDes){

	try{
		int nofTag=0;
		Tokenizer str = Tokenizer(ElementDes," ");

		this->Id = stoi(str.nextToken()); 
		this->Type = stoi(str.nextToken());
		nofTag= stoi(str.nextToken());
		this->NofTag = nofTag; 

		this->PhysicalTag = stoi(str.nextToken());
		this->EntityTag = stoi(str.nextToken());

		for (int i=0; i<nofTag-2; i++)
			str.nextToken(); 

		while(str.hasMoreTokens()){
			
			this->NodeList.push_back(stoi(str.nextToken()));
		}
	} catch(exception& e) { throw "\033[1;31mERROR:: Gmsh File has invalid symbols in Element Section. Unable to convert string to integer in Gmsh File\033[0m\n" ;}
}

void Element::setId(const int& id){

	this->Id=id;
}

void Element::setType(const int& type){

	this->Type = type;
}

void Element::setNofTag(const int& nofTag){

	this->NofTag = nofTag;
}

void Element::setPhysicalTag(const int& physicalTag){

	this->PhysicalTag = physicalTag;
}

void Element::setEntityTag(const int& entityTag){

	this->EntityTag = entityTag;
}

void Element::setNodeList(vector<int> nodeList){

	this->NodeList  = nodeList;
}

void Element::addNode(int node){

	this->NodeList.push_back(node);
}

int Element::getId(){

	return this->Id;
}

int Element::getType(){

	return this->Type;
}

int Element::getNofTag(){

	return this->NofTag;
}

int Element::getPhysicalTag(){

	return this->PhysicalTag;
}

int Element::getEntityTag(){

	return this->EntityTag;
}

vector<int> Element::getNodeList(){

	return this->NodeList;
}