/****************************************************************************
* 								Mapping.cpp
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

#include "Semantics.h"
#include "Mapping.h"
#include "Tokenizer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

Mapping::Mapping(const string& fileName){

	this->FileName=fileName;
}

Mapping::~Mapping(){}

/******************************************************************************
****************************** Public Function*********************************
******************************************************************************/

void Mapping::setFileName(string fileName){

	this->FileName=fileName;
}

void Mapping::createMapping(){

	this->mapFile();
}

map<string,string> Mapping::getElementMap(){

	return this->ElementMap;
}

set<string> Mapping::getEssiTagList(){

	return this->EssiTagList;
}

map<string,Semantics> Mapping::getFunction(){

	return this->Function;
}

/******************************************************************************
****************************** Purivate Function*********************************
******************************************************************************/

void Mapping::mapFile(){

	fstream mapFile(this->FileName, fstream::in);
	string line;

	cout << "Started Reading" << endl;

	while(getline(mapFile,line)){

		Tokenizer str = Tokenizer(line,"#  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("ELEMENT_ID"))
			break;
	}

	while(getline(mapFile,line)){

		string strLine = delSpaces(line);
		Tokenizer str = Tokenizer(line,"#  \t\v\n\r\f\"");

		if(!str.nextToken().compare("ENDELEMENT_ID"))
			break;
		if(!str.currToken().substr(0,2).compare("//"))
			continue;
		if(delSpaces(str.currToken()).length()==0)
			continue;

		string elementDes="";
		string elementId = "";

		elementId = elementId + str.currToken();
		elementDes= elementDes + str.nextToken();
		str.setDelimiter("\"");
		elementDes= elementDes + str.nextToken();

		this->ElementMap.insert(pair<string,string>(elementId,elementDes));
	}

	while(getline(mapFile,line)){

		Tokenizer str = Tokenizer(line,"#  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("ESSI_TAGS"))
			break;
	}

	while(getline(mapFile,line)){

		Tokenizer str = Tokenizer(line,"#  \t\v\n\r\f");
		if(!delSpaces(str.nextToken()).compare("ENDESSI_TAGS"))
			break;
		if(!delSpaces(str.currToken()).substr(0,2).compare("//"))
			continue;
		if(delSpaces(str.currToken()).length()==0)
			continue;
		
		this->EssiTagList.insert(str.currToken());
	}

	while(getline(mapFile,line)){

		string Id, GmshCommandList, EssiCommand="";
		Tokenizer str = Tokenizer(line,"!  \t\v\n\r\f");

		if(!delSpaces(str.nextToken()).substr(0,2).compare("//"))
			continue;
		if(delSpaces(str.currToken()).length()==0)
			continue;

		Id = str.currToken();
		str.setDelimiter("<>");
		
		if(delSpaces(str.nextToken()).length()==0)
			GmshCommandList = delSpaces(str.nextToken());
		else
			GmshCommandList = delSpaces(str.currToken());

		str.setDelimiter("  \t\v\n\r\f");

		EssiCommand= EssiCommand + delSpaces(str.nextToken())+" ";
		str.setDelimiter("<>");
		EssiCommand= EssiCommand + str.nextToken();

		this->makeFunction(Id, GmshCommandList,EssiCommand);
	}

	mapFile.close();
}

void Mapping::makeFunction(string Id, string GmshCommandList, string EssiCommand){

	Tokenizer str = Tokenizer(GmshCommandList,"|");
	while(str.hasMoreTokens()){

		Semantics semantic = Semantics( str.nextToken(),EssiCommand);
		semantic.setElementId(Id);
		this->Function.insert(pair<string,Semantics> (semantic.getEssiTag(),semantic));
	}
}

void Mapping::addElementId(string id, string des){

	this->ElementMap.insert(pair<string,string>(id,des));
}

void Mapping::addEssiTag(string EssiTag){

	this->EssiTagList.insert(EssiTag);
}

void Mapping::addFunction(string GmshCommand, Semantics& semantic){

	this->Function.insert(pair<string,Semantics>(GmshCommand,semantic));
}

string Mapping::delSpaces(string str){

   str.erase(remove(str.begin(), str.end(), ' '), str.end());
   return str;
}