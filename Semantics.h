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

#include "Tokenizer.h"

class Semantics{

	public:

		Semantics();
		Semantics(const string& Gmsh, const string& Essi);
		Semantics(const string& Gmsh, const string& Essi, const int& id);
		~Semantics();
		void setSemantics(const string& Gmsh, const string& Essi);
		void setElementId(const string& id);
		void setSemanticsId(const int& id);
		void setEssiTag(const string& tag);
		void setEssiTagList(set<string> TagList);
		string getEssiTag();
		set<string> getEssiTagList();
		int getNofEssiVariables();
		int getNofGmshVariables();
		int getSemanticsId();
		string getElementId();
		bool getMatchMode();
		vector<string> getTagList();
		vector<string> getVarList();
		
	private:

		void setMatchMode();
		void setEssiCommand(const string& Command);
		void setGmshCommand(const string& Command);
		string ElementId ="";
		int SemanticsId=0;
		bool MatchMode=true;
		string EssiTag ="";
		string EssiCommand="";
		string GmshCommand="";
		int nofEssiVariables=0;
		int nofGmshVariables=0;
		vector<string> TagList;
		vector<string> VarList;
		set<string> EssiTagList;
		// string init[]= {"element", "damping", "displacement", "field", "load", "material", "motion", "node", "nodes" };
};

#endif //SEMANTICS_H