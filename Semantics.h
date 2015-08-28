/*********************************************************************************************************
*  File:        Sematics.h          	        		| Copyright:: Version of Creative Commons License*
*  Description: Builds up the semantics from mapping.fei| Boris Jeremic,                                 *
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
#ifndef SEMANTICS_H
#define SEMANTICS_H

#include <vector>
#include <set>
#include <map>

#include "Tokenizer.h"

class Semantics{

	public:

		Semantics();
		Semantics(const string& Gmsh, const string& Essi);
		Semantics(const string& Gmsh, const string& Essi, const string& id);
		~Semantics();
		void setSemantics(const string& Gmsh, const string& Essi);
		void setElementId(const string& id);
		void setSemanticsId(const string& id);
		void setEssiTagList(set<string> TagList);
		set<string> getEssiTagList();
		int getNofEssiVariables();
		int getNofGmshVariables();
		int getNofTagVariables();
		string getSemanticsId();
		string getElementId();
		string getEssiTag();
		bool getMatchMode();
		map<string,int> getTagList();
		vector<string> getVarList();
		vector<string> getEssiVarList();
		string getEssiCommand();
		string getGmshCommand();
		
	private:

		void setEssiTag(const string& tag);
		void setMatchMode();
		void setEssiCommand(const string& Command);
		void setGmshCommand(const string& Command);
		void setNofTagVariables();
		string delSpaces(string str);
		string ElementId ="";
		string SemanticsId="";
		bool MatchMode=true;
		string EssiTag ="";
		string EssiCommand="";
		string GmshCommand="";
		int NofEssiVariables=0;
		int NofGmshVariables=0;
		int NofTagVariables=0;
		map<string, int> TagList;
		vector<string> VarList;
		vector<string> EssiVarList;
		set<string> EssiTagList;
};

#endif //SEMANTICS_H