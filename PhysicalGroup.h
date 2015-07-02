/*****************************************************************************
* 								EPhysicalGroup.h
* 								================
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/
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
		// void setCommandList(vector<string> commandList);
		// void setVariableList(vector<vector<string>> variableList);
		int getId();
		int getType();
		string getPhysicTag();
		vector<string> getCommandList();
		vector<vector<string>> getVariableList();
		
	private:

		void setContents(const string& PhysicDes);
		void Process(const string& Command);
		string delSpaces( string& str);
		int Id;
		int Type;
		string PhysicTag;
		vector<string>   CommandList;
		vector<vector<string>> VariableList;
};


#endif //PHYSICALGROUP_H