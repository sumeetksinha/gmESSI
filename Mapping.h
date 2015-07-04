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

const string DEFAULTMAPFILE = "mapping.fei";

class Mapping{

	public:

		Mapping(const string& FileName=DEFAULTMAPFILE);
		~Mapping();

		void setFileName(string filename);
		void createMapping();
		map<string,string> getElementMap();
		set<string> getEssiTagList();
		map<string,Semantics> getFunction();

	private:

		void mapFile();
		void addElementId(string id, string des);
		void addEssiTag(string EssiTag);
		void addFunction(string GmshCommand, Semantics& semantic);
		void makeFunction(string Id, string GmshCommandList, string EssiCommand);
		string delSpaces(string str);
		string FileName = DEFAULTMAPFILE;
		map<string,string> ElementMap;
		map<string,Semantics> Function;
		set<string> EssiTagList;
};

#endif //MAPPING_H