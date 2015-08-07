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
		Element(const int& id, const int& type, const int& nofTag, const int& physicalTag, const int& entityTag, vector<int> nodeList);
		Element(const string ElementDes);
		~Element();
		void setElement(const string ElementDes);
		void setId(const int& id);
		void setType(const int& type);
		void setNofTag(const int& nofTag);
		void setPhysicalTag(const int& physicalTag);
		void setEntityTag(const int& entityTag);
		void setNodeList(vector<int> nodeList);
		void addNode(int node);
		int getId();
		int getType();
		int getNofTag();
		int getEntityTag();
		int getPhysicalTag();
		vector<int> getNodeList();
		
		//Just for Boost sake of 

		bool operator==(const Element& other) {return false;}
  		bool operator!=(const Element& other) {return true;}

	private:

		int Id;
		int Type;
		int NofTag;
		int EntityTag;
		int PhysicalTag;
		vector<int> NodeList;
};


#endif //ELEMENT_H