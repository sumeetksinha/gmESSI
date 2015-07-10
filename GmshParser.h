/****************************************************************************
* 								GMSHParser.h
* 								============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

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
		vector<Node> getNodeList();
		vector<Element> getElementList();
		vector<PhysicalGroup> getPhysicalGroupList();
		int getNewPhysicalGroup();
		int getNewNode();
		int getNewElement();

	private:

		void parseGmshFile();
		string delSpaces(string str);
		string FileName;
		map<int,Node>  NodeMap;
		map<int,NodeElement> PhysicalGroupMap;
		map<int,NodeElement> EntityMap;
		vector<Node> NodeList;
		vector<Element> ElementList;
		vector<PhysicalGroup> PhysicalGroupList;
		int newNode, newElement , newPhysicalGroup;
};

#endif //GMSHPARSER_H