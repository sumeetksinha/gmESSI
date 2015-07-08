/*****************************************************************************
*                               GmshTranslator.h
*                               ======================
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/

#ifndef GMSHTRANSLATOR_H
#define GMSHTRANSLATOR_H

#include "Tokenizer.h"
#include "Semantics.h"
#include "PhysicalGroup.h"
#include "Element.h"
#include "Mapping.h"
#include "GmshParser.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <algorithm>


using namespace::std;

class GmshTranslator{

    public:

        GmshTranslator();
        GmshTranslator(const string& gmshFile, const string& newDir);
        GmshTranslator(const string& gmshFile, const string& mapppingFile, const string& newDir);
        ~GmshTranslator();

        void setGmshFile(const string& gmshFile, const string& newDir);
        void Convert();
        string getFileName();

    private:

        void GmshToEssi();
        string getVariable(string& var);
        void clear( queue<string> &q );
        string delSpaces(string str);
        string PrintEssiCommand(string Command, int NofEssiVariables, int j);
        string GmshFile, MappingFile, pwd, geometryFile, mainFile, loadFile;

        void AddNodeCommand(const int&i, const int& j);
        void ElementalCommand(const int& i, const int& j);
        void ElementalCompoundCommand(const int& i, const int& j);
        void NodalCommand(const int& i, const int& j);
        void GeneralElementalCommand(const int& i, const int& j);
        void SingularCommand(const int& i, const int& j);

        Mapping Map = Mapping("mapping.fei");
        GmshParser GmshParse = GmshParser ();
        vector<PhysicalGroup> PhysicalGroupList;
        map<string,Semantics> FunctionMap;
        set<string> EssiTagList;
        map<string,Semantics>::iterator FunctionIter;
        map<int,NodeElement> PhysicalGroupMap;
        map<int,NodeElement> EntityMap;
        map<int,Node> NodeMap;

        vector<string> CommandList;
        vector<vector<string>> VariableList;
        vector<int> NofVariablesList;
        vector<string> UserCommandList;

        map<string,int> EssiTagVariableMap;
        queue<string> TempVariable;
};

#endif //GMSHTRANSLATOR_H