/********************************************************************************************************
*  File:        PhysicalGroup.cpp           	        | Copyright:: ##############################    *
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

#include "PhysicalGroup.h"
#include <iostream>
#include <string>
#include <algorithm>
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

	// cout << "Command String :: " << Command << endl;

	int nofTokens = 0, nofVariables=0; 
	vector<string> varList;
	string essiTag="";
	// Tokenizer inpString = Tokenizer(Command,"  \t\v\n\r\f{,}()");
	Tokenizer inpString = Tokenizer(trim(Command),"{,}");
	nofTokens = inpString.countTokens()-1;
	nofVariables = nofTokens-2;
	essiTag = essiTag + trim(inpString.nextToken()) + "{";

	for( int i=0 ;i<nofTokens-2; i++){
		string variable= trim(inpString.nextToken());
		if(!variable.compare("")){
			nofVariables = nofVariables -1;
			continue;
		}
		varList.push_back(variable);
		essiTag = essiTag + " ,";
	}

	string variable= trim(inpString.nextToken());
	varList.push_back(variable);

	if(inpString.hasMoreTokens()){
		
		string nextvariable= trim(inpString.nextToken());

		if(nextvariable.compare("")){
			nofVariables++;
			essiTag = essiTag + " ,";
			varList.push_back(nextvariable);
		}
	}

	if(variable.compare("")){
		nofVariables++;
	}

	if(nofVariables == 0 )	nofVariables=1;
	else if (nofVariables < 0 ){ nofVariables=0; vector<string> emptyvarList; varList = emptyvarList;}

	essiTag = essiTag + " }"+to_string(nofVariables);
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
