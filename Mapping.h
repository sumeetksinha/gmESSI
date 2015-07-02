/*****************************************************************************
* 								Mapping.h
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/
#ifndef MAPPING_H
#define MAPPING_H

#include <map>
#include <set>

#include "Tokenizer.h"
#include "Semantics.h"

const string DEFAULTMAPFILE = "Mapping.fei";

class Mapping{

	public:

		Mapping();
		Mapping(const string& FileName=DEFAULTMAPFILE);
		~Mapping();
		void addElementId(string id, string des);
		void addEssiTag(string EssiTag);
		void addFunction(string GmshCommand, Semantics semantic);
		map<string,string> getElementMap();
		set<string> getEssitagList();
		map<string,Semantics> getFunction();

	private:

		void mapFile();
		string FileName;
		map<string,string> ElementMap;
		map<string,Semantics> Function;
		set<string> EssiTagList;
};

#endif //MAPPING_H