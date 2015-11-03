/*********************************************************************************************************
*  File:        GmshParser.cpp          	        	| Copyright:: Version of Creative Commons License*
*  Description: A parser for Gmsh       				| Boris Jeremic,                                 *
*  Rev:         Version 1                               | University of California Davis                 *
*  Created:     June 28, 2015                           | California                                     *
*  Author:      Sumeet Kumar Sinha                      | jeremic@ucdavis.edu                            *
*  Email:       sumeet.kumar507@gmail.com               |                                                *
*                                                                                                        *
*  The copyright to the computer program(s) herein is the property of Sumeet Kumar Sinha, Boris Jeremic  *
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha       *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which       *
*  the program(s) have been supplied.                                                                    *
*********************************************************************************************************/

#include "GmshParser.h"
#include <iostream>
#include <fstream>
#include <algorithm>

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

GmshParser::GmshParser(){}

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

map<int,NodeElement> GmshParser::getPhysicalGroupMap(){

	return this->PhysicalGroupMap;
}

map<int,NodeElement> GmshParser::getEntityMap(){

	return this->EntityMap;
}

map<string,int> GmshParser::getPhysicalStringNameToIdMap(){

	return this->PhysicalStringNameToIdMap;
}
		
vector<Node> GmshParser::getNodeList(){

	return this->NodeList;
}

vector<Element> GmshParser::getElementList(){

	return this->ElementList;
}

void GmshParser::addElement(Element elm){
	return this->ElementList.push_back(elm);	
}

vector<PhysicalGroup> GmshParser::getPhysicalGroupList(){

	return this->PhysicalGroupList;
}

int GmshParser::getNewPhysicalGroup(){

	return this->newPhysicalGroup;
}

int GmshParser::getNewNode(){

	return this->newNode;
}

int GmshParser::getNewElement(){

	return this->newElement;
}

int GmshParser::getNewEntity(){

	int new_entity = 0;
	for (map<int,NodeElement>::iterator it=this->EntityMap.begin(); it!=this->EntityMap.end(); ++it){
		if(it->first >= new_entity) new_entity = it->first;
	}
	return new_entity;
}

/******************************************************************************
****************************** Purivate Function*******************************
******************************************************************************/

void GmshParser::parseGmshFile(){

	fstream parseFile(this->FileName, fstream::in);
	string line;

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("MeshFormat"))
			break;
	}

	if(parseFile.eof()) return; 

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("EndMeshFormat"))
			break;
		if(line.compare("2.2 0 8")){

			string msg = "\033[1;31mERROR:: Currently this software supports only  MSH ASCII 2.2 0 8 Format\033[0m\n" ; 
            throw msg.c_str();
		} 
	}

	if(parseFile.eof()) return; 

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("PhysicalNames"))
			break;
	}

	if(parseFile.eof()) return; 

	getline(parseFile,line);
	newPhysicalGroup = stof(line)+1;

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("EndPhysicalNames"))
			break;
		PhysicalGroup PhyGroup = PhysicalGroup(line);
		this->PhysicalGroupList.push_back(PhyGroup);
		this->PhysicalStringNameToIdMap.insert(pair<string,int>(PhyGroup.getPhysicTag(),PhyGroup.getId()));
		this->PhysicalStringNameToIdMap.insert(pair<string,int>(to_string(PhyGroup.getId()),PhyGroup.getId()));
	}

	if(parseFile.eof()) return; 

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("Nodes"))
			break;
	}

	if(parseFile.eof()) return; 

	getline(parseFile,line);
	newNode = stof(line)+1;

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("EndNodes"))
			break;
		Node node = Node(line);
		// this->NodeList.push_back(node);
		this->NodeMap.insert(pair<int,Node>(node.getId(),node));
	}

	if(parseFile.eof()) return; 

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("Elements"))
			break;
	}

	if(parseFile.eof()) return; 

	getline(parseFile,line);
	newElement = stof(line)+1;

	while(getline(parseFile,line)){

		Tokenizer str = Tokenizer(line,"$  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("EndElements"))
			break;
		Element elm = Element(line);
		this->ElementList.push_back(elm);

		int PhysicalTag = elm.getPhysicalTag() , EntityTag = elm.getEntityTag() ;

		if(PhysicalTag!=0){

			map<int,NodeElement>::iterator it = this->PhysicalGroupMap.find(PhysicalTag);

			if (it != this->PhysicalGroupMap.end()){

				it->second.ElementList.push_back(elm);
				int nofNode = elm.getNodeList().size();

				for(int j = 0 ; j <nofNode ; j++){

					int node = elm.getNodeList().at(j);
					map<int,int>::iterator find= it->second.NodeList.find(node);
					if (!(find != it->second.NodeList.end()))
						it->second.NodeList.insert(pair<int,int>(node,node));
				}
			}

			else{
				struct NodeElement newNodeElement;

				newNodeElement.ElementList.push_back(elm);
				int nofNode = elm.getNodeList().size();

				for(int j = 0 ; j <nofNode ; j++){

					int node = elm.getNodeList().at(j);
					newNodeElement.NodeList.insert(pair<int,int>(node,node));
				}

				this->PhysicalGroupMap.insert(pair<int,NodeElement>(PhysicalTag,newNodeElement));
			}

		}

		if(EntityTag!=0){

			map<int,NodeElement>::iterator it = this->EntityMap.find(EntityTag);

			if (it != this->EntityMap.end()){

				it->second.ElementList.push_back(elm);
				int nofNode = elm.getNodeList().size();

				for(int j = 0 ; j <nofNode ; j++){

					int node = elm.getNodeList().at(j);
					map<int,int>::iterator find= it->second.NodeList.find(node);
					if (!(find != it->second.NodeList.end()))
						it->second.NodeList.insert(pair<int,int>(node,node));
				}
			}

			else{
				struct NodeElement newNodeElement;

				newNodeElement.ElementList.push_back(elm);
				int nofNode = elm.getNodeList().size();

				for(int j = 0 ; j <nofNode ; j++){

					int node = elm.getNodeList().at(j);
					newNodeElement.NodeList.insert(pair<int,int>(node,node));
				}

				this->EntityMap.insert(pair<int,NodeElement>(EntityTag,newNodeElement));
			}

		}
	}

	if(parseFile.eof()) return; 
	parseFile.close();
}

string GmshParser::delSpaces(string str){

   str.erase(remove(str.begin(), str.end(), ' '), str.end());
   return str;
}
