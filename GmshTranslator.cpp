/********************************************************************************************************
*  File:        GmshTranslator.cpp                      | Copyright:: ##############################    *
*  Description: Contains the magic of translation       | BOX 1505                                      *
*  Rev:         Version 1                               | 125 25 ALVSJO                                 *
*  Created:     June 28, 2015                           | SWEDEN                                        *
*  Author:      Sumeet Kumar Sinha                      | tel int 530-601-8271                          *
*  Email:       sumeet.kumar507@gmail.com               |                                               *
*                                                                                                       *
*  The copyright to the computer program(s) herein is the property of #######################, ######.  *
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha      *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which      *
*  the program(s) have been supplied.                                                                   *
********************************************************************************************************/

#include "GmshTranslator.h"
#include "OctParser.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <queue>
#include <cmath>
#include <algorithm>
#include <iomanip> 
#include <iomanip> 
#include <sstream>

using namespace::std;

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

GmshTranslator::GmshTranslator(){}

GmshTranslator::GmshTranslator(const string& gmshFile, const string& newDir){

    GmshFile = gmshFile+".msh";
    this->pwd = newDir+gmshFile; 
    geometryFile = newDir + gmshFile + "_geometry.fei";
    loadFile = newDir + gmshFile + "_load.fei";
    mainFile = newDir + gmshFile + "_analysis.fei";
}

GmshTranslator::GmshTranslator(const string& gmshFile, const string& mappingFile, const string& newDir){

    GmshFile = gmshFile+".msh";
    MappingFile = mappingFile;
    this->pwd = newDir+gmshFile; 
    geometryFile = newDir + gmshFile + "_geometry.fei";
    loadFile = newDir + gmshFile + "_load.fei";
    mainFile = newDir + gmshFile + "_analysis.fei";
}

GmshTranslator::~GmshTranslator(){}

/******************************************************************************
**************************** Public Functions *********************************
******************************************************************************/

void GmshTranslator::setGmshFile(const string& gmshFile, const string& newDir){

    this->GmshFile = gmshFile+".msh";
    this->pwd = newDir+gmshFile; 
    geometryFile = newDir + gmshFile + "_geometry.fei";
    loadFile = newDir + gmshFile + "_load.fei";
    mainFile = newDir + gmshFile + "_analysis.fei";;
}

void GmshTranslator::Convert(){

    this->GmshToEssi();
}

string GmshTranslator::getFileName(){

    return this->GmshFile;
}

/******************************************************************************
**************************** Private Functions *********************************
******************************************************************************/

void GmshTranslator::GmshToEssi(){

    ofstream MainFile(mainFile,ios::out);  

    MainFile << "\n" <<"model name \"" << this->GmshFile << "\";\n";
    MainFile << "\n" <<"include \"" << this->geometryFile << "\";\n";
    MainFile << "\n" <<"new loading stage \"" << "Stage_1 Loading" <<"\";\n";
    MainFile << "\n" <<"include \"" << this->loadFile << "\";\n";

    MainFile.close();
    ofstream GeometryFile(geometryFile, ios::out); GeometryFile.close();
    ofstream LoadFile(loadFile,ios::out); LoadFile.close();

    this->Map.createMapping();
    this->GmshParse.setFileName(this->GmshFile);
    this->GmshParse.Parse();

    this->PhysicalGroupList = this->GmshParse.getPhysicalGroupList();
    this->FunctionMap = this->Map.getFunction();
    this->EssiTagList = this->Map.getEssiTagList();
    this->PhysicalGroupMap = this->GmshParse.getPhysicalGroupMap();
    this->EntityMap = this->GmshParse.getEntityMap();
    this->NodeMap = this->GmshParse.getNodeMap();

    this->EssiTagVariableMap.insert(pair<string,int>("element",GmshParse.getNewElement()));
    this->EssiTagVariableMap.insert(pair<string,int>("node",GmshParse.getNewNode()));
    this->EssiTagVariableMap.insert(pair<string,int>("nodes",GmshParse.getNewNode()));
    this->EssiTagVariableMap.insert(pair<string,int>("damping",1));
    this->EssiTagVariableMap.insert(pair<string,int>("displacement",1));
    this->EssiTagVariableMap.insert(pair<string,int>("field",1));
    this->EssiTagVariableMap.insert(pair<string,int>("load",1));
    this->EssiTagVariableMap.insert(pair<string,int>("material",1));
    this->EssiTagVariableMap.insert(pair<string,int>("motion",1));

    int PhysicalGroupListSize = this->PhysicalGroupList.size();
    // cout << "PhysicalGroupListSize " << PhysicalGroupListSize<< endl << endl;

    for (int i=0; i<PhysicalGroupListSize; i++){

        this->CommandList = this->PhysicalGroupList.at(i).getCommandList();
        this->VariableList = this->PhysicalGroupList.at(i).getVariableList();
        this->NofVariablesList = this->PhysicalGroupList.at(i).getNofVariables();
        this->UserCommandList = this->PhysicalGroupList.at(i).getUserCommandList();
        int CommandListSize = CommandList.size();

        // cout << this->PhysicalGroupList.at(i).getId() << endl;

        for (int j=0; j<CommandListSize; j++){
            
            this->FunctionIter = this->FunctionMap.find(this->CommandList.at(j));
            //printing output
            cout << left << setw(40) << this->UserCommandList.at(j) ;

            if (this->FunctionIter != this->FunctionMap.end()){
                
                cout<< left << setw(15) << "Found!!";
                // cout<< "Element Id !!" << this->FunctionIter->second.getElementId() <<endl;

                if(this->FunctionIter->second.getMatchMode() && !(this->FunctionIter->second.getSemanticsId().compare("c")))
                    this->ElementalCompoundCommand(i,j);
                else if(this->FunctionIter->second.getMatchMode())
                    this->ElementalCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("nc"))
                    this->NodalCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("ec"))
                    this->GeneralElementalCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("sc"))
                    this->SingularCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("an"))
                    this->AddNodeCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("cc"))
                    this->ConnectCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("mv"))
                    this->MaterialVariationalCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("wr"))
                    this->WriteCommand(i,j);
            }
            else{
                
                cout << left << setw(15) << "\033[1;31mNot Found!!" << "\033[0m";
                cout << "\t" << "\033[1;33mWARNING:: Execuation of the command escaped. The essi command \'" << this->UserCommandList.at(j) << "\'" << "could not be found" << " \033[0m\n" ; 
            }
        }
    } 

    UpdateGmshFile();

    PhysicalGroup PythonScriptCommands = PhysicalGroup();
    this->PhysicalGroupList.push_back(PythonScriptCommands);

    map<string,int>::iterator EssiTagIterBegin = EssiTagVariableMap.begin();
    map<string,int>::iterator EssiTagIterEnd = EssiTagVariableMap.end();

    cout <<endl << endl<< "\033[1;36m************************ Updated New Tag Numberring *********************" << "\033[0m\n";
    for(map<string,int>::iterator it = EssiTagIterBegin ; it!= EssiTagIterEnd ; ++it){

        cout << "\033[1;36m" << setw(15) << it->first << " = " << it->second << "\033[0m\n";
    }
}

void GmshTranslator::AddNodeCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile,ios::app); int init=0;int nofRun=0;
    GeometryFile << PrintStartConversion(j);

    if(!(this->UserCommandList.at(j).substr(3,3).compare("All"))){

        map<int,Node> ::iterator AllNodeBegin = this->NodeMap.begin();
        map<int,Node> ::iterator AllNodeEnd = this->NodeMap.end();

        for(map<int,Node>::iterator it=AllNodeBegin; it!=AllNodeEnd; ++it){
        
            this->TempVariable.push(to_string(it->second.getId()));nofRun++;
            this->TempVariable.push(to_string(it->second.getXcord())+"*"+this->VariableList.at(j).at(0)); 
            this->TempVariable.push(to_string(it->second.getYcord())+"*"+this->VariableList.at(j).at(0));
            this->TempVariable.push(to_string(it->second.getZcord())+"*"+this->VariableList.at(j).at(0)); 
            this->TempVariable.push(this->VariableList.at(j).at(1)); 

            GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
        }

        GeometryFile << PrintEndConversion(nofRun,j);
        GeometryFile.close();
        return;
    }

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j).at(0),i,j,init);
    /// Ends Initialization here

    map<int,int> NodeList = TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    // int NofVariables = NofVariablesList.at(j);
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();

    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){
        
        int n = init; nofRun++;
        map<int,Node>::iterator NodeInfo = this->NodeMap.find(it->second);

        this->TempVariable.push(to_string(it->second));
        this->TempVariable.push(to_string(NodeInfo->second.getXcord())+"*"+this->VariableList.at(j).at(n)); 
        this->TempVariable.push(to_string(NodeInfo->second.getYcord())+"*"+this->VariableList.at(j).at(n));
        this->TempVariable.push(to_string(NodeInfo->second.getZcord())+"*"+this->VariableList.at(j).at(n)); 
        this->TempVariable.push(Variables.at(n+1)); 

        GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    GeometryFile << PrintEndConversion(nofRun,j);
    GeometryFile.close();
}

void GmshTranslator::ElementalCommand(const int& i, const int& j){
  
    ofstream GeometryFile(geometryFile, ios::app); int init=0;
    GeometryFile << PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j).at(0),i,j,init);
    /// Ends Initialization here

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    // int NofVariables = this->NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    int nofRun=0; 

    for(int k =0;k<ElementListSize ; k++){

        int m =0,n=init; 

        if( !(this->FunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

            nofRun++;

            for(int l=0 ; l<NofEssiVariables ;l++ ){

                Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                string var = tknzr.nextToken();
             
                if(!var.compare("element")){
                    this->TempVariable.push(to_string(ElementList.at(k).getId()));
                }
                else if(!var.compare("node") || !var.compare("nodes")){
                    this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
                }
                else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                    this->TempVariable.push(this->getVariable(var)); 
                }
                else {string newVar = Variables.at(n++);
                    this->TempVariable.push(newVar);
                    UpdateEssiTags(newVar,l);
                }
            }

            GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
        }
    }

    GeometryFile << PrintEndConversion(nofRun,j);
    GeometryFile.close();
}

void GmshTranslator::ElementalCompoundCommand(const int& i, const int& j){

	cout << "ElementalCompoundCommand" << endl;
 
    ofstream LoadFile(loadFile,ios::app); int init=0;
    LoadFile<< PrintStartConversion(j);

    // cout << init << endl;

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j).at(0),i,j,init);
    /// Ends Initialization here

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    // int NofVariables = this->NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    Tokenizer tkr = Tokenizer(Variables.at(init),"#");
    string type = this->delSpaces(tkr.nextToken());

    // cout << Variables.at(init) << endl;

    if(!tkr.hasMoreTokens()){
        
        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has a syntaxERROR in Enty/Phy# tag" + " \033[0m\n" ;
        throw msg.c_str();
    }

    int tag = stoi(this->delSpaces(tkr.nextToken()));
        
    map<int,NodeElement>::iterator EntityMapIter = this->EntityMap.find(tag);
    map<int,NodeElement>::iterator PhysicalMapIter = this->PhysicalGroupMap.find(tag);

    int nofRun=0;

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=init+1;

        if( !(this->FunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

            nofRun++;

            for(int l=0 ; l<NofEssiVariables ;l++ ){

                Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                string var = tknzr.nextToken();
             
                if(!var.compare("element")){
                    this->TempVariable.push(to_string(ElementList.at(k).getId()));
                }
                else if(!var.compare("node") || !var.compare("nodes")){

                    bool loop=true; int loopMax =  ElementList.at(k).getNodeList().size();

                    while(loop && m<loopMax){
                         
                        if(!type.compare("Phy")){

                            if(PhysicalMapIter==this->PhysicalGroupMap.end()){

                                string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no such Physical/Entity Group" + " \033[0m\n";
                                throw msg.c_str(); 
                            }

                            map<int,int>::iterator NodeIter = PhysicalMapIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));

                            // if(NodeIter==PhysicalMapIter->second.NodeList.end()){

                            //     string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
                            //     cout <<  msg;   
                            // }

                            map<int,int>::iterator NodeIterEnd = PhysicalMapIter->second.NodeList.end();

                            if(NodeIter!= NodeIterEnd){
                                this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
                                loop = false;
                            }

                            m++;
                        }

                        else if (!type.compare("Enty")){

                            if(EntityMapIter==this->EntityMap.end()){

                                string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no such Physical/Entity Group" + " \033[0m\n";
                                throw msg.c_str(); 
                            }

                            map<int,int>::iterator NodeIter = EntityMapIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));

                            // if(NodeIter==PhysicalMapIter->second.NodeList.end()){

                            //     string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
                            //     cout << msg;   
                            // }

                            map<int,int>::iterator NodeIterEnd = EntityMapIter->second.NodeList.end();

                            if(NodeIter!= NodeIterEnd){
                               this-> TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
                                loop = false;
                            }
                            
                            m++;
                        }

                        else{

                            string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ; 
                            throw msg.c_str();
                        }
                    }
                }
                else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){ 
                    this->TempVariable.push(this->getVariable(var)); 
                }
                else {
                    string newVar = Variables.at(n++);
                    this->TempVariable.push(newVar);
                    UpdateEssiTags(newVar,l);
                }
            }

            if (this->TempVariable.size() == this->FunctionIter->second.getNofEssiVariables())  
                LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
            else{

                this->clear(this->TempVariable);
                for(int l=0 ; l<NofEssiVariables ;l++ ){

                    Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                    string var = tknzr.nextToken();
                    map<string,int>:: iterator itr = this->EssiTagVariableMap.find(var);
                    if(!var.compare("node") || !var.compare("nodes")) continue;
                    else if (itr != this->EssiTagVariableMap.end()) itr->second = itr->second -1; 
                }

            }
        }
    }

    LoadFile << PrintEndConversion(nofRun,j);
    LoadFile.close();
}

void GmshTranslator::NodalCommand(const int& i, const int& j){

    ofstream LoadFile(loadFile,ios::app); int init =0;
    LoadFile<< PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j).at(0),i,j,init);
    /// Ends Initialization here

    map<int,int> NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    // int NofVariables = NofVariablesList.at(j);
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    int nofRun=0;

    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){

        int n=init;

        for(int l=0 ; l<NofEssiVariables ;l++ ){

            nofRun++;

            Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
            string var = tknzr.nextToken();
             
            if(!var.compare("node") || !var.compare("nodes")){
                this->TempVariable.push(to_string(it->second));  
            }
            else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                this->TempVariable.push(this->getVariable(var)); 
            }
            else {string newVar = Variables.at(n++);
                this->TempVariable.push(newVar);
                UpdateEssiTags(newVar,l);
            }
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    LoadFile << PrintEndConversion(nofRun,j);
    LoadFile.close();
}

void GmshTranslator::GeneralElementalCommand(const int& i, const int& j){

    ofstream LoadFile(loadFile,ios::app); int init=0;
    LoadFile<< PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j).at(0),i,j,init);
    /// Ends Initialization here

    vector<Element> ElementList =TypeIter->second.ElementList;
    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    // int NofVariables = NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    int nofRun=0;

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=init ;

        for(int l=0 ; l<NofEssiVariables ;l++ ){

            nofRun++;

            Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
            string var = tknzr.nextToken();
           
            if(!var.compare("element")){
                this->TempVariable.push(to_string(ElementList.at(k).getId()));
            }
            else if(!var.compare("node") || !var.compare("nodes")){
                this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
            }
            else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                this->TempVariable.push(this->getVariable(var)); 
            }
            else {string newVar = Variables.at(n++);
                this->TempVariable.push(newVar);
                UpdateEssiTags(newVar,l);
            }
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    LoadFile << PrintEndConversion(nofRun,j);
    LoadFile.close();
}

void GmshTranslator::SingularCommand(const int& i, const int& j){

    ofstream MainFile(mainFile,ios::app);

    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    // int NofVariables = NofVariablesList.at(j);
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
    int n=0;  

    for(int l=0 ; l<NofEssiVariables ;l++ ){

        Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
        string var = tknzr.nextToken();
    
        if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
           this->TempVariable.push(this->getVariable(var));
        }
        else {string newVar = Variables.at(n++);
            this->TempVariable.push(newVar);
            UpdateEssiTags(newVar,l); 
        }
    }

    MainFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);

    cout << "Sucessfully Converted" << endl;
    MainFile.close();
}

void GmshTranslator::ConnectCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile,ios::app);
    vector<string> Variables = VariableList.at(j);

    Tokenizer tkr = Tokenizer(Variables.at(0),"#");
    string type1 = this->delSpaces(tkr.nextToken());
    string tag1 = this->delSpaces(tkr.nextToken());
    map<int,NodeElement>::iterator Iterator1;
    if(!type1.compare("Enty")) Iterator1 = this->EntityMap.find(stoi(tag1));
    else if(!type1.compare("Phy")) Iterator1 = this->PhysicalGroupMap.find(stoi(tag1));
    else{string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ; 
        throw msg.c_str();}
    map<int,int>::iterator Iterator1Begin = Iterator1->second.NodeList.begin();
    map<int,int>::iterator Iterator1End = Iterator1->second.NodeList.end();

    tkr.set(Variables.at(1),"#");
    string type2 = this->delSpaces(tkr.nextToken());
    string tag2 = this->delSpaces(tkr.nextToken());
    map<int,NodeElement>::iterator Iterator2;
    if(!type2.compare("Enty")) Iterator2 = this->EntityMap.find(stoi(tag2));
    else if(!type2.compare("Phy")) Iterator2 = this->PhysicalGroupMap.find(stoi(tag2));
    else{string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ; 
        throw msg.c_str();}
    map<int,int>::iterator Iterator2Begin = Iterator2->second.NodeList.begin();
    map<int,int>::iterator Iterator2End = Iterator2->second.NodeList.end();

    tkr.set(Variables.at(2),"#");
    string type3 = this->delSpaces(tkr.nextToken());
    string tag3 = this->delSpaces(tkr.nextToken());
    map<int,NodeElement>::iterator Iterator3;
    if(!type3.compare("Enty")) Iterator3 = this->EntityMap.find(stoi(tag3));
    else if(!type3.compare("Phy")) Iterator3 = this->PhysicalGroupMap.find(stoi(tag3));
    else{string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ; 
        throw msg.c_str();}
    map<int,int>::iterator Iterator3Begin = Iterator3->second.NodeList.begin();
    map<int,int>::iterator Iterator3End = Iterator3->second.NodeList.end();

    string vector = (this->delSpaces(Variables.at(3))); tkr.set(vector,";");
    double vec_x = stof(tkr.nextToken()),vec_y = stof(tkr.nextToken()),vec_z = stof(tkr.nextToken());double sum = vec_x*vec_x+vec_y*vec_y+vec_z*vec_z;
    vec_x=vec_x/sqrt(sum);vec_y=vec_y/sqrt(sum);vec_z=vec_z/sqrt(sum);
    double length = stof(this->delSpaces(Variables.at(4))); //magnitude of vector
    int NofLayers = stoi(this->delSpaces(Variables.at(5)));
    int algo = stoi(this->delSpaces(Variables.at(6)));
    double tolerence = stof(this->delSpaces(Variables.at(7))); 

    if(Iterator1==this->EntityMap.end()||Iterator2==this->EntityMap.end()||(Iterator3==this->EntityMap.end() && algo<=0)||Iterator1==this->PhysicalGroupMap.end()||Iterator2==this->PhysicalGroupMap.end()||(Iterator3==this->PhysicalGroupMap.end() && algo<=0)){

        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no such Physical/Entity Group" + " \033[0m\n";
        throw msg.c_str(); 
    }

    map<int,Node> ::iterator NodeMap1, NodeMap2;
    struct NodeElement newNodeElement;

    int Node1=0,Node2=0;

    // cout << algo << endl;

    for(map<int,int>::iterator It1 = Iterator1Begin; It1!=Iterator1End ;++It1){

        NodeMap1 = this->NodeMap.find(It1->second);

        for(int i=1 ; i<NofLayers; i++){

            double node1_x = NodeMap1->second.getXcord()+length*(vec_x);
            double node1_y = NodeMap1->second.getYcord()+length*(vec_y);
            double node1_z = NodeMap1->second.getZcord()+length*(vec_z);

            if(algo>0){
                //create node           
                string str = "node";
                Node newNode = Node(stoi(getVariable(str)), node1_x, node1_y, node1_z );
                this->NodeMap.insert(pair<int,Node>(newNode.getId(),newNode)); str = "element";
                Node1=NodeMap1->first;
                Node2=newNode.getId(); 

                cout << newNode.getId() << " " << node1_x << " " << node1_y << " " << node1_z << endl;
                // cout << " New Nodes and Elements Created" << endl; 
                string ElementDesc = this->getVariable(str)+ " 1 2 "  + to_string(this->PhysicalGroupMap.size()+1) + " -1 " + to_string(Node1) + " " + to_string(Node2);
                cout << ElementDesc << endl;
                Element newElement = Element(ElementDesc);
                this->GmshParse.addElement(newElement);
                newNodeElement.ElementList.push_back(newElement);
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                NodeMap1 = this->NodeMap.find(newNode.getId());
            }
            else{
                //find node

                for(map<int,int>::iterator It3 = Iterator3Begin; It3!=Iterator3End ;++It3){

                    NodeMap2 = this->NodeMap.find(It3->second);

                    double modx = abs(node1_x-NodeMap2->second.getXcord());
                    double mody = abs(node1_y-NodeMap2->second.getYcord());
                    double modz = abs(node1_z-NodeMap2->second.getZcord());

                    if(sqrt(modx*modx+ mody*mody +modz*modz)<=tolerence){

                        Node1=NodeMap1->first;
                        Node2=It3->second; string str = "element";
                        // cout << "New Elements Created from finding nodes" << endl; 
                        string ElementDesc = this->getVariable(str)+ " 1 2 "  + to_string(this->PhysicalGroupMap.size()+1) + " -1 " + to_string(Node1) + " " + to_string(Node2);
                        cout << ElementDesc << endl;
                        Element newElement = Element(ElementDesc);
                        this->GmshParse.addElement(newElement);
                        newNodeElement.ElementList.push_back(newElement);
                        newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                        newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                        break;
                    }
                }
                NodeMap1 = NodeMap2;
            }
        }

        double node1_x = NodeMap1->second.getXcord()+length*(vec_x);
        double node1_y = NodeMap1->second.getYcord()+length*(vec_y);
        double node1_z = NodeMap1->second.getZcord()+length*(vec_z);


        for(map<int,int>::iterator It2 = Iterator2Begin; It2!=Iterator2End ;++It2){

            NodeMap2 = this->NodeMap.find(It2->second);
            double modx = abs(node1_x-NodeMap2->second.getXcord());
            double mody = abs(node1_y-NodeMap2->second.getYcord());
            double modz = abs(node1_z-NodeMap2->second.getZcord());

            if(sqrt(modx*modx+ mody*mody +modz*modz)<=tolerence){
                    
                Node1=NodeMap1->first;                    
                Node2=It2->second; string str = "element";
                // cout << "New Elements Created" << endl; 
                string ElementDesc = this->getVariable(str) + " 1 2 "  + to_string(this->PhysicalGroupMap.size()+1) + " -1 " + to_string(Node1) + " " + to_string(Node2);
                cout << ElementDesc << endl;
                Element newElement = Element(ElementDesc);
                this->GmshParse.addElement(newElement);
                newNodeElement.ElementList.push_back(newElement);
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                break;
            }
        }
    }

    this->PhysicalGroupMap.insert(pair<int,NodeElement>(this->PhysicalGroupMap.size()+1,newNodeElement));
    string PhysicDes = "1 "+ to_string(this->PhysicalGroupMap.size())+  " \"NewLineGroup_" + to_string(this->PhysicalGroupMap.size()) +  "\""; 
    PhysicalGroup newPhysicalGroup = PhysicalGroup(PhysicDes);
    this->PhysicalGroupList.push_back(newPhysicalGroup);

    cout << "Sucessfully Converted" << endl;
    cout << "\033[1;36mNew Physical Group " << this->PhysicalGroupMap.size() << " consisting of Nodes and 2-noded elements created "  << "\033[0m\n";
    GeometryFile.close();
}

void GmshTranslator::MaterialVariationalCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile, ios::app); int init =1;
    ofstream MainFile(mainFile, ios::app);
    GeometryFile << PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j).at(0),i,j,init);
    /// Ends Initialization here

    map<string,int>::iterator MaterialTagIter;
    map<string,Semantics>::iterator TempFunctionIter;

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    // int NofVariables = this->NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
     
    string gmshCommandtag = Variables.at(0);
    Tokenizer tkr =   Tokenizer(this->UserCommandList.at(j),"{");tkr.nextToken();tkr.nextToken();tkr.nextToken();
    string str = delSpaces(tkr.currToken()); string arguments="";
    if(str.at(0)!='}') arguments = ","+Variables.at(init++); replace( arguments.begin(), arguments.end(), ';', ',' );
    string ElementalCommand =  gmshCommandtag +"{1"+ arguments +"}";
    PhysicalGroup TempPhyGroup = PhysicalGroup(); TempPhyGroup.Process(ElementalCommand);
    TempFunctionIter = this->FunctionMap.find(TempPhyGroup.getCommandList().at(0));

    if (TempFunctionIter == this->FunctionMap.end()){
        cout << left << setw(15) << "\033[1;31mNot Found!!" << "\033[0m";
        cout << "\t" << "\033[1;33mWARNING:: Execuation of the command escaped. The essi command \'" << this->UserCommandList.at(j) << "\'" << "could not be found" << " \033[0m\n" ;
        return;
    }

    int nofRun=0;

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=init, MatTag=0;

        if( !(TempFunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

            nofRun++;

            int ElemNodeSize = ElementList.at(k).getNodeList().size();
            double x_cord=0, y_cord=0, z_cord=0;

            for(int z=0 ; z<ElemNodeSize ;z++ ){

                map<int,Node>::iterator NodeInfo = this->NodeMap.find(ElementList.at(k).getNodeList().at(z));
                x_cord = x_cord + NodeInfo->second.getXcord();
                y_cord = y_cord + NodeInfo->second.getYcord();
                z_cord = z_cord + NodeInfo->second.getZcord();
            }

            x_cord = x_cord/ElemNodeSize; y_cord = y_cord/ElemNodeSize; z_cord = z_cord/ElemNodeSize;
            string ScriptVariables = "x =" + to_string(x_cord) + ";" +  "y =" +   to_string(y_cord) + ";" + "z =" + to_string(z_cord) + ";";

            string Material=this->FunctionIter->second.getEssiCommand();

            for(int l=0 ; l<NofEssiVariables ;l++ ){

                if (!EssiVariables.at(l).compare("material#1")){
                   string ScriptFunction = "material";
                   this->TempVariable.push(this->getVariable(ScriptFunction));
                }
                else{

                    string ScriptFunction = Variables.at(n++);
                    string function = ScriptVariables + ScriptFunction + ";";
                    string prec = Variables.at(n++) ; 
                    string var = to_string(roundToSignificantFigures(stof(this->Evaluate.eval(function)), stoi(prec)));          //running script in octave
                    Material = Material + " " + var;
                    this->TempVariable.push(var);  
                }
            }

            MaterialTagIter = this->MaterialTag.find(Material);

            if(MaterialTagIter!=this->MaterialTag.end()){
                MatTag = MaterialTagIter->second;
                clear( this->TempVariable );
                this->EssiTagVariableMap.find("material")->second = this->EssiTagVariableMap.find("material")->second-1;
            }
            else{
                MainFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
                MatTag = this->EssiTagVariableMap.find("material")->second;
                this->MaterialTag.insert(pair<string,int>(Material,MatTag));
            }

            ElementalCommand = gmshCommandtag +"{"+to_string(MatTag)+arguments+"}";
            PhysicalGroup ElemPhyGroup = PhysicalGroup(); ElemPhyGroup.Process(ElementalCommand);
            TempFunctionIter = this->FunctionMap.find(ElemPhyGroup.getCommandList().at(0));

            vector<string> NewVariables = ElemPhyGroup.getVariableList().at(0);
            vector<string> NewEssiVariables= TempFunctionIter->second.getVarList();
            int NewNofEssiVariables = TempFunctionIter->second.getNofEssiVariables();

            n=0;m=0;

            for(int l=0 ; l<NewNofEssiVariables ;l++ ){

                Tokenizer tknzr = Tokenizer(NewEssiVariables.at(l),"#");
                string var = tknzr.nextToken();
             
                if(!var.compare("element")){
                    this->TempVariable.push(to_string(ElementList.at(k).getId()));
                }
                else if(!var.compare("node") || !var.compare("nodes")){
                    this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
                }
                else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                    this->TempVariable.push(this->getVariable(var)); 
                    // cout<<this->getVariable(var)<< endl;
                }
                else{string newVar = NewVariables.at(n++);
                    this->TempVariable.push(newVar);
                    UpdateEssiTags(newVar,l);  
                }
            }

            GeometryFile << this->PrintEssiCommand(TempFunctionIter->second.getEssiCommand(),TempFunctionIter->second.getNofEssiVariables(),j);
        }
    }

    GeometryFile << PrintEndConversion(nofRun,j);
    GeometryFile.close();
    remove( "script" );
    MainFile.close();
}

void GmshTranslator::WriteCommand(const int&i, const int& j){

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter; int init=0;
    setTypeIter(TypeIter,this->VariableList.at(j).at(0),i,j,init);
    /// Ends Initialization here

    vector<string> Variables = VariableList.at(j);
    string NodesFileName = this->pwd +"_" + VariableList.at(j).at(init) +"_Nodes.txt";
    string ElementsFileName = this->pwd +"_" + VariableList.at(j).at(init) +"_Elements.txt";
    ofstream NodesFile(NodesFileName,ios::out); 
    ofstream ElementsFile(ElementsFileName,ios::out); 

    vector<Element> ElementList = TypeIter->second.ElementList;
    int ElementListSize = ElementList.size();

    for(int i=0; i<ElementListSize ; i++){
        
        ElementsFile << ElementList.at(i).getId() << "\t";
        int size =  ElementList.at(i).getNodeList().size();

        for( int j =0 ;j<size ; j++ )            
            ElementsFile << ElementList.at(i).getNodeList().at(j) << "\t";
        ElementsFile << "\n";
    }

    map<int,int> NodeList =TypeIter->second.NodeList;
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();

    int nofRun=0;
    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){
        
        nofRun++;
        map<int,Node>::iterator NodeInfo = this->NodeMap.find(it->second);

        NodesFile << to_string(NodeInfo->second.getId()) << "\t";
        NodesFile << to_string(NodeInfo->second.getXcord())<< "\t"; 
        NodesFile << to_string(NodeInfo->second.getYcord())<< "\t"; 
        NodesFile << to_string(NodeInfo->second.getZcord())<< "\n"; 
    }

    if(nofRun==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    cout << "Sucessfully Converted" << endl;
    NodesFile.close();
    ElementsFile.close();
}


string GmshTranslator::getVariable(string& var){

    map<string,int>::iterator EssiTagIter = EssiTagVariableMap.find(var);

    if(!var.compare("nodes")){

        map<string,int>::iterator  FindIter = EssiTagVariableMap.find("node");
        FindIter->second++;
    }
    else if(!var.compare("node")){

        map<string,int>::iterator FindIter = EssiTagVariableMap.find("nodes");
        FindIter->second++;
    }

    return  to_string(EssiTagIter->second++);
}

string GmshTranslator::PrintEssiCommand(string Command, int NofEssiVariables, int j){

    stringstream Ecommand; int nofTokens = 0;
    Tokenizer inpString = Tokenizer(Command,"{}") ;

    if(this->TempVariable.size() > NofEssiVariables) {
        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + "has more than required variables" + " \033[0m\n" ; 
        throw msg.c_str();
    }

    if(this->TempVariable.size() < NofEssiVariables) {        
        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + "has less than required variables" + " \033[0m\n" ; 
        throw msg.c_str();
    }
    
    while(inpString.hasMoreTokens() && nofTokens++<NofEssiVariables){
        Ecommand << inpString.nextToken()  << setw(10);
        Ecommand << TempVariable.front()   << setw(10) ;
        TempVariable.pop();
    }
    
    Ecommand <<inpString.nextToken() << setw(10) <<"\n";
    return Ecommand.str();
}

void GmshTranslator::clear( queue<string> &q ){

   queue<string> empty;
   swap( q, empty );
}

string GmshTranslator::delSpaces(string str){

   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}

double GmshTranslator::roundToSignificantFigures(double num, int n) {
    
  double RoundedNumber =0;
  
  if(n >=1)  RoundedNumber = floor((num+ 5* pow(10,-n-1))*pow(10,n))/pow(10,n);
  else RoundedNumber = floor((num+5* pow(10,-(n+2)))/pow(10,-n-1))*pow(10,-n-1);

  return RoundedNumber;
}

void GmshTranslator::setTypeIter(map<int,NodeElement>::iterator &TypeIter,const string& variable,const int& i,const int& j, int &n){

    Tokenizer tknzr = Tokenizer(variable," #");

    if(tknzr.countTokens()==2){

        string type = delSpaces(tknzr.nextToken());
        int tag = stoi(delSpaces(tknzr.nextToken()));

        if(!type.compare("Enty")){

            TypeIter = this->EntityMap.find(tag);

            if(TypeIter==this->EntityMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
                cout << msg;
                return;        
            }
        }

        else if (!type.compare("Phy")){

            TypeIter = this->PhysicalGroupMap.find(tag);

            if(TypeIter==this->PhysicalGroupMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
                cout << msg;
                return;        
            }
        }

        else{

            string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has a syntaxERROR in Enty/Phy# tag" + " \033[0m\n" ;
            throw msg.c_str();
        }

        n=n+1;
    }

    else{

        TypeIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

        if(TypeIter==this->PhysicalGroupMap.end()|| TypeIter->second.NodeList.size()==0){
            string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
            cout << msg;
            return;        
        }
    }

    return;
}

string GmshTranslator::PrintStartConversion(const int& j){

    string str = "\n//*************************************************************************************************************************\n";
    str = str + "//\t\t\t\t\t\t\t" +  this->UserCommandList.at(j) + "Begins\n";
    str = str + "//*************************************************************************************************************************\n\n" ;
    return str;
}

string GmshTranslator::PrintEndConversion(const int& nofRun, const int& j){

    if(nofRun==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
        cout << msg;     
    }

    cout << "Sucessfully Converted" << endl;

    string str = "\n//*************************************************************************************************************************\n";
    str = str + "//\t\t\t\t\t\t\t" +  this->UserCommandList.at(j) + "Ends\n";
    str = str + "//*************************************************************************************************************************\n\n" ;
    return str;
}

void GmshTranslator::UpdateEssiTags(const string& newVar, const int& l){

    Tokenizer temptknzr = Tokenizer(this->FunctionIter->second.getEssiVarList().at(l),"#");
    string tempvar = temptknzr.nextToken(); 
    map<string,int>::iterator EssiTagVariablemapIter = this->EssiTagVariableMap.find(tempvar);
    if (EssiTagVariablemapIter!= this->EssiTagVariableMap.end())   EssiTagVariablemapIter->second = stoi(newVar)+1;
}

void GmshTranslator::Convert(const string& GmssiCommand){

    int i = this->PhysicalGroupList.size()-1;
    PhysicalGroupList.at(i).Process(GmssiCommand);

    this->CommandList = this->PhysicalGroupList.at(i).getCommandList();
    this->VariableList = this->PhysicalGroupList.at(i).getVariableList();
    this->NofVariablesList = this->PhysicalGroupList.at(i).getNofVariables();
    this->UserCommandList = this->PhysicalGroupList.at(i).getUserCommandList();
    int j = CommandList.size()-1;     
    this->FunctionIter = this->FunctionMap.find(this->CommandList.at(j));

    cout << left << setw(40) << this->UserCommandList.at(j) ;

    if (this->FunctionIter != this->FunctionMap.end()){
        
        cout<< left << setw(15) << "Found!!";

        if(this->FunctionIter->second.getMatchMode() && !(this->FunctionIter->second.getSemanticsId().compare("c")))
            this->ElementalCompoundCommand(i,j);
        else if(this->FunctionIter->second.getMatchMode())
            this->ElementalCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("nc"))
            this->NodalCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("ec"))
            this->GeneralElementalCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("sc"))
            this->SingularCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("an"))
            this->AddNodeCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("cc"))
            this->ConnectCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("mv"))
            this->MaterialVariationalCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("wr"))
            this->WriteCommand(i,j);
    }
    else{
        
        cout << left << setw(15) << "\033[1;31mNot Found!!" << "\033[0m";
        cout << "\t" << "\033[1;33mWARNING:: Execuation of the command escaped. The essi command \'" << this->UserCommandList.at(j) << "\'" << "could not be found" << " \033[0m\n" ; 
    }

    map<string,int>::iterator EssiTagIterBegin = EssiTagVariableMap.begin();
    map<string,int>::iterator EssiTagIterEnd = EssiTagVariableMap.end();

    cout <<endl << endl<< "\033[1;36m************************ Updated New Tag Numberring *********************" << "\033[0m\n";
    for(map<string,int>::iterator it = EssiTagIterBegin ; it!= EssiTagIterEnd ; ++it){

        cout << "\033[1;36m" << setw(15) << it->first << " = " << it->second << "\033[0m\n";
    }
}

void GmshTranslator::UpdateGmshFile(){

	string UpdatedGmshFile = this->pwd +".msh";
    ofstream UpdateGmshFile(UpdatedGmshFile,ios::out); 

    int PhysicalGroupListSize = this->PhysicalGroupList.size();

    UpdateGmshFile << "$MeshFormat \n2.2 0 8 \n$EndMeshFormat\n" << "$PhysicalNames\n" << PhysicalGroupListSize << "\n";

    for (int i=0; i<PhysicalGroupListSize; i++)
		UpdateGmshFile << this->PhysicalGroupList.at(i).getPhysicDes()<< endl;

    UpdateGmshFile << "$EndPhysicalNames\n" << "$Nodes\n" << this->NodeMap.size() << "\n";

	map<int,Node> ::iterator AllNodeBegin = this->NodeMap.begin();
    map<int,Node> ::iterator AllNodeEnd = this->NodeMap.end();

    for(map<int,Node>::iterator it=AllNodeBegin; it!=AllNodeEnd; ++it)
        UpdateGmshFile << it->second.getId() << " " << it->second.getXcord() << " " << it->second.getYcord() << " " << it->second.getZcord()  << "\n"; 

    vector<Element>  ElementList= this->GmshParse.getElementList();
    int ElementListSize =  ElementList.size();

    UpdateGmshFile << "$EndNodes\n" << "$Elements\n" << ElementListSize << "\n";

    for (int i=0; i<ElementListSize; i++){

		UpdateGmshFile << ElementList.at(i).getId() << " " << ElementList.at(i).getType() << " " << ElementList.at(i).getNofTag() << " " <<  ElementList.at(i).getPhysicalTag() << " " << ElementList.at(i).getEntityTag();
		vector<int> NodeList = ElementList.at(i).getNodeList(); 
		int NodeListSize = NodeList.size();
		for (int j=0; j<NodeListSize; j++)
			UpdateGmshFile << " " << NodeList.at(j);
		UpdateGmshFile << "\n";
	}
	UpdateGmshFile << "$EndElements\n";
}