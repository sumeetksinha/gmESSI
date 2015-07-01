/*****************************************************************************
* 								Semantics.cpp
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

#include "Semantics.h"
#include <iostream>
#include <algorithm>

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

Semantics::Semantics(){

	this->EssiTagList.insert("element");this->EssiTagList.insert("damping");this->EssiTagList.insert("displacement");
	this->EssiTagList.insert("field");this->EssiTagList.insert("load");this->EssiTagList.insert("material");
	this->EssiTagList.insert("motion");this->EssiTagList.insert("node");this->EssiTagList.insert("nodes");
}

Semantics::Semantics(const string& Gmsh, const string& Essi){

	this->EssiTagList.insert("element");this->EssiTagList.insert("damping");this->EssiTagList.insert("displacement");
	this->EssiTagList.insert("field");this->EssiTagList.insert("load");this->EssiTagList.insert("material");
	this->EssiTagList.insert("motion");this->EssiTagList.insert("node");this->EssiTagList.insert("nodes");
	this->setSemantics(Gmsh, Essi);

}

Semantics::Semantics(const string& Gmsh, const string& Essi, const int& id){

	this->EssiTagList.insert("element");this->EssiTagList.insert("damping");this->EssiTagList.insert("displacement");
	this->EssiTagList.insert("field");this->EssiTagList.insert("load");this->EssiTagList.insert("material");
	this->EssiTagList.insert("motion");this->EssiTagList.insert("node");this->EssiTagList.insert("nodes");
	this->setSemantics(Gmsh, Essi);
	this->setSemanticsId(id);
}

Semantics::~Semantics(){}

/******************************************************************************
****************************** Public Function*********************************
******************************************************************************/

void Semantics::setSemantics(const string& Gmsh, const string& Essi){

	this->setEssiCommand(Essi);
	this->setGmshCommand(Gmsh);
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

vector<string> Semantics::getVarList(){

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
	while( inpString.hasMoreTokens()){
		Ecommand = Ecommand + inpString.nextToken()+" ";
	}
	
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

			set<string>::iterator it = this->EssiTagList.find(variable);
			if (it != this->EssiTagList.end()) {
				this->TagList.push_back(*it);
			}

			prevTag= temp.currToken();
		}
		
		this->VarList.push_back(variable);
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
		set<string>::iterator it = this->EssiTagList.find(inpString.currToken());
		if (it != this->EssiTagList.end()) {
			vector<string>::iterator it;
			it = find(this->VarList.begin(),this->VarList.end(),inpString.currToken());
			if (it != this->VarList.end())
				*it = "variable";
		}

		Gcommand = Gcommand +" ,";
	}

	Gcommand = Gcommand +" }";
	this->GmshCommand= Gcommand;
}
