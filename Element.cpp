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

Element::Element(const int& id, const int& type, const int& nofTag, vector<int> tagList, vector<int> nodeList){

	this->Id = id;
	this->Type = type;
	this->NofTag = nofTag;
	this->TagList = tagList;
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

	for (int i=0; i<nofTag; i++)
		this->TagList.push_back(stoi(str.nextToken())); 

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

void Element::setTagList(vector<int> tagList){

	this->TagList = tagList;
}

void Element::setNodeList(vector<int> nodeList){

	this->NodeList  = nodeList;
}

void Element::addTag(int tag){

	this->TagList.push_back(tag);
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

vector<int> Element::getTagList(){

	return this->TagList;
}

vector<int> Element::getNodeList(){

	return this->NodeList;
}