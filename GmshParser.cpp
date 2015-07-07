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

GmshParser::GmshParser(){

}

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

	parseFile.close();
}

string GmshParser::delSpaces(string str){

   str.erase(remove(str.begin(), str.end(), ' '), str.end());
   return str;
}
