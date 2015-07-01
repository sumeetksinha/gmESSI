/*****************************************************************************
* 								Element.h
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/
#ifndef ELEMENT_H
#define ELEMENT_H

#include "Node.h"
#include <vector>

class Element{

	public:

		Element();
		Semantics(const int& Id, const int& Type, const int& NofTag, vector<int> TagList, vector<Node> NodeList);
		~Element();
		void setId(const int& id);
		void setType(const int& type);
		void setNofTag(const int& nofTag);
		void setTagList(vector<int> tagList);
		void setNodeList(vector<Node> nodeList);
		void addTag(int tag);
		void addNode(Node node);
		void getId();
		void getType();
		void getNofTag();
		void getTagList();
		void getNodeList();
		
	private:

		int Id;
		int Type;
		int NofTag;
		vector<int>   TagList;
		vector<Node> NodeList;
};


#endif //ELEMENTS_H