/********************************************************************************************************
*  File:        PhysicalGroup.h 	           	        | Copyright:: ##############################    *
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
#ifndef PHYSICALGROUP_H
#define PHYSICALGROUP_H

#include "Tokenizer.h"
#include <vector>

class PhysicalGroup{

	public:

		PhysicalGroup();
		PhysicalGroup(const string& PhysicDes);
		~PhysicalGroup();
		void setPhysicalGroup(const string& PhysicDes);
		void setId(const int& id);
		void setType(const int& type);
		void setPhysicTag(const string& physicTag);
		void Process(const string& Command);
		int getId();
		int getType();
		string getPhysicTag();
		string getPhysicDes();
		vector<string> getCommandList();
		vector<string> getUserCommandList();
		vector<vector<string>> getVariableList();
		vector<int> getNofVariables();

	private:

		void setContents(const string& PhysicDes);
		string delSpaces( string str);
		int Id;
		int Type;
		string PhysicDes;
		string PhysicTag;
		vector<string> UserCommandList;
		vector<string> CommandList;
		vector<vector<string>> VariableList;
		vector<int> NofVariables;
};


#endif //PHYSICALGROUP_H