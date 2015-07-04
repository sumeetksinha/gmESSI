/****************************************************************************
* 								GMSHParser.h
* 								============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

#include "GmshParser.h"
#include <iostream>
#include <fstream>
#include <algorithm>

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

GmshParser::GmshParser(const string& filename){

	this->FileName = filename;
}

GmshParser::~GmshParser(){}

/******************************************************************************
****************************** Public Function*********************************
******************************************************************************/

void GmshParser::setFileName(string filename){

	this->FileName = filename;
}

void GmshParser::Parse(){

	this->parseGmshFile();
}

map<int,Node> GmshParser::getNodeMap(){

	return this->NodeMap;
}

vector<Node> GmshParser::getNodeList(){

	return this->NodeList;
}

vector<Element> GmshParser::getElementList(){

	return this->ElementList;
}

vector<PhysicalGroup> GmshParser::getPhysicalGroupList(){

	return this->PhysicalGroupList;
}

int GmshParser::getNewPhysicalGroup(){

	return this->newPhysicalGroup++;
}

int GmshParser::getNewNode(){

	return this->newNode++;
}

int GmshParser::getNewElement(){

	return this->newElement++;
}

/******************************************************************************
****************************** Purivate Function*******************************
******************************************************************************/

void GmshParser::parseGmshFile(){

	fstream parseFile(this->FileName, fstream::in);
	string line;

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("PhysicalNames"))
			break;
	}

	getline(parseFile,line);
	newPhysicalGroup = stof(line)+1;

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("EndPhysicalNames"))
			break;
		PhysicalGroup PhyGroup = PhysicalGroup(line);
		this->PhysicalGroupList.push_back(PhyGroup);
	}

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("Nodes"))
			break;
	}

	getline(parseFile,line);
	newNode = stof(line)+1;

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("EndNodes"))
			break;
		Node node = Node(line);
		this->NodeList.push_back(node);
		this->NodeMap.insert(pair<int,Node>(node.getId(),node));
	}

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("Elements"))
			break;
	}

	getline(parseFile,line);
	newElement = stof(line)+1;

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("EndElements"))
			break;
		Element elm = Element(line);
		this->ElementList.push_back(elm);
	}

	parseFile.close();
}

string GmshParser::delSpaces(string str){

   str.erase(remove(str.begin(), str.end(), ' '), str.end());
   return str;
}
