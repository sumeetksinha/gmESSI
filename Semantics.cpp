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

Semantics::Semantics(const string& Gmsh, const string& Essi, const string& id){

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

void Semantics::setSemanticsId(const string& id){

	this->SemanticsId = id;
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

	return this-> NofEssiVariables;
}

int Semantics::getNofGmshVariables(){

	return this-> NofGmshVariables;
}

int Semantics::getNofTagVariables(){

	return this-> NofTagVariables;
}

string Semantics::getSemanticsId(){

	return this->SemanticsId;
}

string Semantics::getElementId(){
	
	return this->ElementId;
}

bool Semantics::getMatchMode(){

	return this->MatchMode;
}

map<string,int> Semantics::getTagList(){

	return this->TagList;
}

vector<string> Semantics::getVarList(){

	return this->VarList;
}

vector<string> Semantics::getEssiVarList(){

	return this->EssiVarList;
}

string Semantics::getEssiCommand(){

	return this->EssiCommand;
}

string Semantics::getGmshCommand(){

	return this->GmshCommand;
}

/******************************************************************************
****************************** Private Function********************************
******************************************************************************/

void  Semantics::setEssiTag(const string& tag){

	this->EssiTag = tag;
}

void Semantics::setMatchMode(){

	Tokenizer tknzr = Tokenizer(this->ElementId,"-");
	this->ElementId = tknzr.nextToken();
	string str= tknzr.currToken();
	if(str.empty() || ((!isdigit(str[0])) && (str[0] != '-') && (str[0] != '+'))) this->MatchMode=false ;
   	char * p ;
   	strtol(str.c_str(), &p, 10) ;
   	this->MatchMode = (*p == 0) ;

   	while(tknzr.hasMoreTokens()){
   		this->SemanticsId = tknzr.nextToken();
   		break;
   	}
}

void Semantics::setEssiCommand(const string& Command){

	int nofTokens = 0;
	string Ecommand = "";
	Tokenizer inpString = Tokenizer(Command," ") ;
	while( inpString.hasMoreTokens()){
		Ecommand = Ecommand + inpString.nextToken()+" ";
	}
	this->EssiCommand = Command;
	
	inpString.set(Command,"{}");
	nofTokens = inpString.countTokens()-1;

	inpString.set(Command,"{}#()=");
	string prevTag = "variable";

	while(inpString.hasMoreTokens() && nofTokens-->0){

		string variable;
		Tokenizer Var = Tokenizer(inpString.nextToken()," ,");

		if(!this->delSpaces(inpString.currToken()).compare(";")) break;                        // Termination Condition with ";"

		Var.setMode(1);
		Var.setcurrPos(inpString.currToken().length()-1);
		string currTag = this->delSpaces(Var.nextToken());

		if (currTag.length()<=1)
			variable = prevTag; 		
		else{
			variable = currTag;
			prevTag= currTag;
		}

		set<string>::iterator it = this->EssiTagList.find(variable);
		if (it != this->EssiTagList.end()) {

			map<string,int>::iterator it = this->TagList.find(variable);

			if (it != this->TagList.end()){
				it->second=it->second+1;
				variable = variable + "#" +to_string(it->second);
			}
			else{
				this->TagList.insert(pair<string,int>(variable,1));
				variable = variable + "#1";
			}

			this->NofTagVariables++;
		}

		//cout << variable << " ";
		
		this->VarList.push_back(variable);
		this->EssiVarList.push_back(variable);
		// inpString.setDelimiter("{}#()=");
	}

	//cout << endl;

	this->NofEssiVariables = this->VarList.size();
}

void Semantics::setGmshCommand(const string& Command){

	int nofTokens = 0;
	string Gcommand = "", essiTag="";
	Tokenizer inpString = Tokenizer(Command," {,;}()");
	nofTokens = inpString.countTokens()-1;
	this->NofGmshVariables = nofTokens-1;
	Gcommand = Gcommand + inpString.nextToken() + "{ ";
	essiTag = essiTag + inpString.currToken() + "{";
	
	for( int i=0 ;i<nofTokens-1; i++){

		string variable= this->delSpaces(inpString.nextToken());
		
		vector<string>::iterator it;
		it = find(this->VarList.begin(),this->VarList.end(),variable);
		
		if (it != this->VarList.end()) 
 			*it = "variable";

		Gcommand = Gcommand +variable+" ,";
		essiTag = essiTag + " ,";
	}

	string variable= this->delSpaces(inpString.nextToken());

	if(variable.compare("")){
		this->NofGmshVariables++;
	}

	vector<string>::iterator it;
	it = find(this->VarList.begin(),this->VarList.end(),variable);
	if (it != this->VarList.end()) 
 		*it = "variable";
 	
	Gcommand = Gcommand +variable + " }";
	essiTag = essiTag + " }"+to_string(this->NofGmshVariables);
	
	this->GmshCommand= Gcommand;
	this->setEssiTag(essiTag);

}

string Semantics::delSpaces(string str){
   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}
