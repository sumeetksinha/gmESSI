/*********************************************************************************************************
*  File:        PhysicalGroup.cpp          	        	| Copyright:: Version of Creative Commons License*
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

#include "PhysicalGroup.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>

using namespace::std;

string trim(const string& str, const string& delimiters = " \f\n\r\t\v" ){
	string s=str;
    s.erase( s.find_last_not_of( delimiters ) + 1 ).erase( 0, s.erase( s.find_last_not_of( delimiters ) + 1 ).find_first_not_of( delimiters ) );
    return s;
}

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

PhysicalGroup::PhysicalGroup(){}

PhysicalGroup::PhysicalGroup(const string& PhysicDes){

	this->setContents(PhysicDes);
}

PhysicalGroup::~PhysicalGroup(){}

/******************************************************************************
**************************** Public Functions *********************************
******************************************************************************/

void PhysicalGroup::setPhysicalGroup(const string& PhysicDes){

	this->setContents(PhysicDes);
}

void PhysicalGroup::setId(const int& id){

	this->Id=id;
}

void PhysicalGroup::setType(const int& type){

	this->Type = type;
}
void PhysicalGroup::setPhysicTag(const string& physicTag){

	this->PhysicTag = physicTag;
}

int PhysicalGroup::getId(){

	return this->Id;
}

string PhysicalGroup::getPhysicDes(){

	return PhysicDes;
}

int PhysicalGroup::getType(){

	return this->Type;
}

string PhysicalGroup::getPhysicTag(){

	return this->PhysicTag;
}

vector<string> PhysicalGroup::getCommandList(){

	return this->CommandList;
}

vector<string> PhysicalGroup::getUserCommandList(){

	return this->UserCommandList;
}

vector<vector<string>> PhysicalGroup::getVariableList(){

	return this->VariableList;
}

vector<int> PhysicalGroup::getNofVariables(){

	return this->NofVariables;
}

/******************************************************************************
**************************** Private Functions ********************************
******************************************************************************/		

void PhysicalGroup::setContents(const string& PhysicDesc){

	this->PhysicDes = PhysicDesc;
	
	Tokenizer str = Tokenizer(PhysicDesc,"  \t\v\n\r\f\"$");
	this->Type = stoi(str.nextToken());
	this->Id = stoi(str.nextToken());
	this->PhysicTag = str.nextToken();

	while(str.hasMoreTokens()){

		str.setDelimiter("[]");
		if(! this->delSpaces(str.nextToken()).compare("\""))
			break;

		if(!str.hasMoreTokens())
		break;

		string comm = this->delSpaces(str.nextToken());
		//cout << comm << endl;

		if(!comm.substr(comm.length()-1,1).compare("\"")){

			string msg = "\033[1;31mERROR:: The command \'" + PhysicDesc + "\'" + " has a syntax error" + " \033[0m\n" ;
        	throw msg.c_str();
		}

		if(!str.hasMoreTokens())
			break;

		this->Process(this->delSpaces(trim(str.currToken())));
	}

}

void PhysicalGroup::Process(const string& Command ){

	string gmESSI_Command = Command, essiTag =""; int nofVariables = 0;
	vector<string> varList;

	boost::regex FunRegex("(\\d|\\w)*\\{");
	boost::sregex_iterator it(gmESSI_Command.begin(), gmESSI_Command.end(), FunRegex);
	essiTag = trim(it->str());

	gmESSI_Command = gmESSI_Command.substr(it->str().length()-1,gmESSI_Command.length());
	gmESSI_Command.erase(std::remove(gmESSI_Command.begin(), gmESSI_Command.end(), '}'), gmESSI_Command.end());
	gmESSI_Command.erase(std::remove(gmESSI_Command.begin(), gmESSI_Command.end(), '{'), gmESSI_Command.end());

	boost::sregex_iterator end;
	boost::regex  ArgRegex("[^,]*\\(([^()]|(?R))*\\)[^,]*|[^,]*");
	boost::sregex_iterator its(gmESSI_Command.begin(), gmESSI_Command.end(), ArgRegex);

	for (; its != end; ++its) 
		if(its->str().compare("")){
    		boost::regex  AssignRegex("[^(:=)]*:=\\s*");
    		string variable = trim(its->str());
			boost::sregex_iterator it(variable.begin(), variable.end(), AssignRegex);
    		if(it!= end)
    			 variable = variable.substr(it->str().length()-1,variable.length());
    		varList.push_back(trim(variable));
    		essiTag = essiTag + " ,";
    		nofVariables++;
		}

	if(nofVariables==0)
    	essiTag = essiTag+ " }"+to_string(nofVariables);
    else 
    	essiTag = essiTag.substr(0,essiTag.length()-1)+ "}"+to_string(nofVariables);

	this->VariableList.push_back(varList);
	this->CommandList.push_back(essiTag);
	this->NofVariables.push_back(nofVariables);
	this->UserCommandList.push_back(Command);

	// cout << Command << endl;
	// cout << essiTag << endl;
}

string PhysicalGroup::delSpaces(string str){
   // str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}
