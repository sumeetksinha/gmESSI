/*********************************************************************************************************
*  File:        Mapping.h          	        			| Copyright:: Version of Creative Commons License*
*  Description: Creates mapping for gmESSI syntax		| Boris Jeremic,                                 *
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