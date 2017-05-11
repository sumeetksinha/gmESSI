/*********************************************************************************************************
*  File:        PhysicalGroup.h          	        	| Copyright:: Version of Creative Commons License*
*  Description: Parses and stores the Physical Group 	| Boris Jeremic,                                 *
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
		int Id=-1;
		int Type;
		string PhysicDes;
		string PhysicTag;
		vector<string> UserCommandList;
		vector<string> CommandList;
		vector<vector<string>> VariableList;
		vector<int> NofVariables;
};


#endif //PHYSICALGROUP_H