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
		Element(const int& id, const int& type, const int& nofTag, vector<int> tagList, vector<int> nodeList);
		Element(const string ElementDes);
		~Element();
		void setElement(const string Element);
		void setId(const int& id);
		void setType(const int& type);
		void setNofTag(const int& nofTag);
		void setTagList(vector<int> tagList);
		void setNodeList(vector<int> nodeList);
		void addTag(int tag);
		void addNode(int node);
		int getId();
		int getType();
		int getNofTag();
		vector<int> getTagList();
		vector<int> getNodeList();
		
	private:

		int Id;
		int Type;
		int NofTag;
		vector<int>   TagList;
		vector<int> NodeList;
};


#endif //ELEMENT_H