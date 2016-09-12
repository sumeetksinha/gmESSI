/********************************************************************************************************
*  File:        Element.h           	        	    | Copyright:: ##############################    *
*  Description: Contains the magic of translation       | BOX 1505                                      *
*  Rev:         Version 1                               | 125 25 ALVSJO                                 *
*  Created:     June 28, 2015                           | SWEDEN                                        *
*  Author:      Sumeet Kumar Sinha                      | tel int 530-601-8271                          *
*  Email:       sumeet.kumar507@gmail.com               |                                               *
*                                                                                                       *
*  The copyright to the computer program(s) herein is the property of #######################, ######.  *
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha      *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which      *
*  the program(s) have been supplied.                                                                   *
********************************************************************************************************/
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