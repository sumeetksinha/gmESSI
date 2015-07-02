/*****************************************************************************
* 								PhysicalGroup.cpp
* 								=================
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

#include "PhysicalGroup.h"
#include <iostream>
#include <string>
#include <algorithm>
using namespace::std;

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
// void setCommandList(vector<string> commandList);
// void setVariableList(vector<vector<string>> variableList);
int PhysicalGroup::getId(){

	return this->Id;
}
int PhysicalGroup::getType(){

	return this->Type;
}
string PhysicalGroup::getPhysicTag(){

	return this->PhysicTag;
}
vector<string> PhysicalGroup::PhysicalGroup::getCommandList(){

	return this->CommandList;
}
vector<vector<string>> PhysicalGroup::getVariableList(){


}

/******************************************************************************
**************************** Private Functions ********************************
******************************************************************************/		

void PhysicalGroup::setContents(const string& PhysicDes){

	Tokenizer str = Tokenizer(PhysicDes," \"$");
	this->Type = stoi(str.nextToken());
	// cout << "Type " << str.currToken() << endl;
	this->Id = stoi(str.nextToken());
	// cout << "Id " << str.currToken() << endl;
	this->PhysicTag = str.nextToken();
	// cout << "PhysicTag " << str.currToken() << endl;

	while(str.hasMoreTokens()){

		str.setDelimiter("<");
		str.nextToken();
		str.setDelimiter(">");
		if(!str.hasMoreTokens()){break;}
		string Command = str.nextToken();
		// cout<<delSpaces(Command)<<endl;
		this->Process(delSpaces(Command));
	}

}

void PhysicalGroup::Process(const string& Command ){

	int nofTokens = 0;
	string Gcommand = "";
	Tokenizer inpString = Tokenizer(Command," {,;}()") ;
	nofTokens = inpString.countTokens()-2;
	// cout << nofTokens << endl;
	Gcommand = Gcommand + inpString.nextToken() + "{";
	
	for( int i=0 ;i<nofTokens-1; i++){

		inpString.nextToken();
		// cout <<  inpString.currToken()<<  " " ;
		// set<string>::iterator it = this->EssiTagList.find(inpString.currToken());
		// if (it != this->EssiTagList.end()) {
		// 	vector<string>::iterator it;
		// 	it = find(this->VarList.begin(),this->VarList.end(),inpString.currToken());
		// 	if (it != this->VarList.end())
		// 		*it = "variable";
		// }

		Gcommand = Gcommand +" ,";
	}
	// cout <<  inpString.nextToken()<<  " " ;
	Gcommand = Gcommand +" }";
	// cout<< Gcommand << endl;
	// this->GmshCommand= Gcommand;
}

string PhysicalGroup::delSpaces(string& str){
   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}
