/****************************************************************************
* 								Mapping.cpp
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

#include "Semantics.h"
#include "Mapping.h"
#include "Tokenizer.h"
#include <iostream>

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

Mapping::Mapping(){ }

Mapping::Mapping(const string& fileName){

	this->FileName=fileName;
}

Mapping::~Mapping(){}

/******************************************************************************
****************************** Public Function*********************************
******************************************************************************/

void Mapping::addElementId(string id, string des){

	this->ElementMap.insert(pair<string,string>(id,des));
}

void Mapping::addEssiTag(string EssiTag){

	this->EssiTagList.insert(EssiTag);
}

void Mapping::addFunction(string GmshCommand, Semantics semantic){

	this->Function.insert(pair<string,Semantics>(GmshCommand,semantic));
}

map<string,string> Mapping::getElementMap(){

	return this->ElementMap;
}

set<string> Mapping::getEssitagList(){

	return this->EssiTagList;
}

map<string,Semantics> Mapping::getFunction(){

	return this->Function;
}

/******************************************************************************
****************************** Purivate Function*********************************
******************************************************************************/

void Mapping::mapFile(){

	
}