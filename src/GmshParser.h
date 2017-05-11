/*********************************************************************************************************
*  File:        GmshParser.h          	        		| Copyright:: Version of Creative Commons License*
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

#ifndef GMSHPARSER_H
#define GMSHPARSER_H

#include <map>
#include <vector>

#include "Tokenizer.h"
#include "PhysicalGroup.h"
#include "Element.h"
#include "Node.h"

struct NodeElement{
	
   vector<Element> ElementList;
   map<int, int> NodeList;

};

class GmshParser{

	public:

		GmshParser();
		GmshParser(const string& FileName);
		~GmshParser();

		void setFileName(string filename);
		void Parse();
		map<int,Node> getNodeMap();
		map<int,NodeElement> getPhysicalGroupMap();
		map<int,NodeElement> getEntityMap();
		map<string,int> getPhysicalStringNameToIdMap();
		vector<Node> getNodeList();
		vector<Element> getElementList();
		void addElement(Element elm);
		vector<PhysicalGroup> getPhysicalGroupList();
		int getNewPhysicalGroup();
		int getNewNode();
		void addNode(Node NewNode); 
		int getNewElement();
		int getNewEntity();

	private:

		void parseGmshFile();
		string delSpaces(string str);
		string FileName;
		map<int,Node>  NodeMap;
		map<int,NodeElement> PhysicalGroupMap;
		map<int,NodeElement> EntityMap;
		map<string,int> PhysicalStringNameToIdMap;
		vector<Node> NodeList;
		vector<Element> ElementList;
		vector<PhysicalGroup> PhysicalGroupList;
		int MaxPhysicalGroupId=0;
		int newNode=1, newElement=1, newPhysicalGroup=1;
};

#endif //GMSHPARSER_H