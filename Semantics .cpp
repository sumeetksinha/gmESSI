/*****************************************************************************
* 								Semantics.cpp
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

#include "Semantics.h"
#include <iostream>

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

Semantics::Semantics(){}

Semantics::Semantics(const string& Gmsh, const string& Essi){

	this->setSemantics(Gmsh, Essi);
}

Semantics::Semantics(const string& Gmsh, const string& Essi, const int& id){

	this->setSemantics(Gmsh, Essi);
	this->setSemanticsId(id);
}

Semantics::~Semantics(){}

/******************************************************************************
****************************** Public Function*********************************
******************************************************************************/

void Semantics::setSemantics(const string& Gmsh, const string& Essi){

	this->setGmshCommand(Gmsh);
	this->setEssiCommand(Essi);
}

void Semantics::setElementId(const string& id){

	this->ElementId = id;
	this->setMatchMode();
}

void Semantics::setSemanticsId(const int& id){

	this->SemanticsId = id;
}

void  Semantics::setEssiTag(const string& tag){

	this->EssiTag = tag;
}

void  Semantics::setEssiTagList(set<string> TagList){

	this->EssiTagList =TagList;
}

string  Semantics::getEssiTag(){
	
	return this->EssiTag;
}

set<string> Semantics::getEssiTagList(){

	return this->EssiTagList;
}

int Semantics::getNofEssiVariables(){

	return this-> nofEssiVariables;
}

int Semantics::getNofGmshVariables(){

	return this-> nofGmshVariables;
}

int Semantics::getSemanticsId(){

	return this->SemanticsId;
}

string Semantics::getElementId(){
	
	return this->ElementId;
}

bool Semantics::getMatchMode(){

	return this->MatchMode;
}

vector<string> Semantics::getTagList(){

	return this->TagList;
}

queue<string> Semantics::getVarList(){

	return this->VarList;
}

/******************************************************************************
****************************** Private Function********************************
******************************************************************************/

void Semantics::setMatchMode(){

	string str= this->ElementId;
	if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+'))) this->MatchMode=false ;
   	char * p ;
   	strtol(str.c_str(), &p, 10) ;
   	this->MatchMode = (*p == 0) ;
}

void Semantics::setEssiCommand(const string& Command){

	int nofTokens = 0;
	string Ecommand = "";
	Tokenizer inpString = Tokenizer(Command," ") ;
	while( inpString.hasMoreTokens())
		Ecommand = Ecommand + inpString.nextToken()+" ";
	
	this->EssiCommand = Command;
	
	inpString.set(Command,"{}#()=");
	string prevTag = "variable";

	while(inpString.hasMoreTokens()){

		string variable;
		Tokenizer Var = Tokenizer(inpString.nextToken()," ,");
		Var.setMode(1);
		Var.setcurrPos(inpString.currToken().length()-1);
		Tokenizer temp = Tokenizer(Var.nextToken()," ;");

		if (temp.nextToken().length()<=1) 
			variable = prevTag;  		
		else{
			variable = temp.currToken();
			// // variable.resize(variable.length());
			// // cout << variable << "  "<<variable.length() << endl;
			// string t = variable.substr(1,variable.size());
			// for(string::iterator it = t.begin(); it != t.end(); ++it) {
			//     cout<<*it; cout << ";";
			// }
			// cout <<endl;
			// // cout << variable.substr(1,variable.length()).compare("element") << endl;
			// cout << *this->EssiTagList.find(variable.data())<< endl;
			// if (this->EssiTagList.find(variable.substr(1,variable.length()))!=this->EssiTagList.end()) {
			// 	this->TagList.push_back(variable);
			// 	cout << "hello";
			// }
			prevTag= temp.currToken();
		}
		
		this->VarList.push(variable);
	}

	this->nofEssiVariables = this->VarList.size();

}

void Semantics::setGmshCommand(const string& Command){

	int nofTokens = 0;
	string Gcommand = "";
	Tokenizer inpString = Tokenizer(Command," {,;}()") ;
	nofTokens = inpString.countTokens()-2;
	this->nofGmshVariables = nofTokens;
	this->setEssiTag(inpString.nextToken());
	Gcommand = Gcommand + inpString.currToken() + "{";
	
	for( int i=0 ;i<nofTokens-1; i++){
		inpString.nextToken();
		Gcommand = Gcommand +" ,";
	}

	Gcommand = Gcommand +" }";
	this->GmshCommand= Gcommand;
}
