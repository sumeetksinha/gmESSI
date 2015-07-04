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

#include "Tokenizer.h"
#include "PhysicalGroup.h"
#include "Element.h"
#include "Node.h"

class GmshParser{

	public:

		GmshParser(const string& FileName);
		~GmshParser();

		void setFileName(string filename);
		void Parse();
		map<int,Node> getNodeMap();
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
		vector<Node> NodeList;
		vector<Element> ElementList;
		vector<PhysicalGroup> PhysicalGroupList;
		int newNode, newElement , newPhysicalGroup;
};

#endif //GMSHPARSER_H