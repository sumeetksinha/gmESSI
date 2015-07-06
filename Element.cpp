/*****************************************************************************
* 								Element.cpp
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

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