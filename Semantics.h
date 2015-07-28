/*****************************************************************************
* 								Semantics.h
* 								=============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/
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