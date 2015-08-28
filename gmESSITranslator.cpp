/*********************************************************************************************************
*  File:        gmESSITranslator.cpp                    | Copyright:: Version of Creative Commons License*
*  Description: Contains the magic of translation       | Boris Jeremic,                                 *
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

#include "gmESSITranslator.h"
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
#include <boost/lexical_cast.hpp>

using namespace::std;

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

gmESSITranslator::gmESSITranslator(){}

gmESSITranslator::gmESSITranslator(const string& gmshFile, const string& newDir){

    GmshFile = gmshFile;
    Tokenizer tknzr = Tokenizer(gmshFile,"/ ."); 
    tknzr.setcurrPos(gmshFile.length()-1);tknzr.setMode(1);tknzr.nextToken();tknzr.nextToken();
    this->pwd = newDir+tknzr.currToken(); 
    geometryFile = newDir + tknzr.currToken() + "_geometry.fei";
    loadFile = newDir + tknzr.currToken() + "_load.fei";
    mainFile = newDir + tknzr.currToken() + "_analysis.fei";
}

gmESSITranslator::gmESSITranslator(const string& gmshFile, const string& mappingFile, const string& newDir){

    GmshFile = gmshFile;
    Tokenizer tknzr = Tokenizer(gmshFile,"/ "); tknzr.setMode(1);tknzr.nextToken();
    MappingFile = mappingFile;
    this->pwd = newDir+tknzr.currToken(); 
    geometryFile = newDir + tknzr.currToken() + "_geometry.fei";
    loadFile = newDir + tknzr.currToken() + "_load.fei";
    mainFile = newDir + tknzr.currToken() + "_analysis.fei";
}

gmESSITranslator::~gmESSITranslator(){ }

/******************************************************************************
**************************** Public Functions *********************************
******************************************************************************/

void gmESSITranslator::setGmshFile(const string& gmshFile, const string& newDir){

    GmshFile = gmshFile;
    Tokenizer tknzr = Tokenizer(gmshFile,"/ "); tknzr.setMode(1);tknzr.nextToken();
    this->pwd = newDir+tknzr.currToken(); 
    geometryFile = newDir + tknzr.currToken() + "_geometry.fei";
    loadFile = newDir + tknzr.currToken() + "_load.fei";
    mainFile = newDir + tknzr.currToken() + "_analysis.fei";
    return;
}

void gmESSITranslator::Convert(){

    this->GmshToEssi();
    return;
}

string gmESSITranslator::getFileName(){

    return this->GmshFile;
}

/******************************************************************************
**************************** Private Functions *********************************
******************************************************************************/

void gmESSITranslator::GmshToEssi(){

    ofstream MainFile(mainFile,ios::out);  

    // MainFile << "\n" <<"model name \"" << this->GmshFile << "\";\n"; MainFile.close();
    ofstream GeometryFile(geometryFile, ios::out); GeometryFile.close();
    ofstream LoadFile(loadFile,ios::out); LoadFile.close();

    this->Map.createMapping();
    this->GmshParse.setFileName(this->GmshFile);
    this->GmshParse.Parse();

    this->PhysicalGroupList = this->GmshParse.getPhysicalGroupList();
    this->FunctionMap = this->Map.getFunction();
    this->EssiTagList = this->Map.getEssiTagList();
    this->PhysicalGroupMap = this->GmshParse.getPhysicalGroupMap();
    this->PhysicalStringNameToIdMap = this->GmshParse.getPhysicalStringNameToIdMap();
    this->EntityMap = this->GmshParse.getEntityMap();
    this->NodeMap = this->GmshParse.getNodeMap();
    this->NewEntity = this->GmshParse.getNewEntity()+1;

    this->EssiTagVariableMap.insert(pair<string,int>("element",this->GmshParse.getNewElement()));
    this->EssiTagVariableMap.insert(pair<string,int>("node",this->GmshParse.getNewNode()));
    this->EssiTagVariableMap.insert(pair<string,int>("nodes",this->GmshParse.getNewNode()));
    this->EssiTagVariableMap.insert(pair<string,int>("damping",1));
    this->EssiTagVariableMap.insert(pair<string,int>("displacement",1));
    this->EssiTagVariableMap.insert(pair<string,int>("field",1));
    this->EssiTagVariableMap.insert(pair<string,int>("load",1));
    this->EssiTagVariableMap.insert(pair<string,int>("material",1));
    this->EssiTagVariableMap.insert(pair<string,int>("motion",1));

    int PhysicalGroupListSize = this->PhysicalGroupList.size();

    for (int i=0; i<PhysicalGroupListSize; i++){

        this->CommandList = this->PhysicalGroupList.at(i).getCommandList();
        this->VariableList = this->PhysicalGroupList.at(i).getVariableList();
        this->NofVariablesList = this->PhysicalGroupList.at(i).getNofVariables();
        this->UserCommandList = this->PhysicalGroupList.at(i).getUserCommandList();
        int CommandListSize = CommandList.size();

        for (int j=0; j<CommandListSize; j++){
            
            this->FunctionIter = this->FunctionMap.find(this->CommandList.at(j));
            cout << left << setw(60) << this->UserCommandList.at(j) ;

            if (this->FunctionIter != this->FunctionMap.end()){
                
                cout<< left << setw(15) << "\t \t Found!!";

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
                
                cout << left << setw(60) << "\033[1;31mNot Found!!" << "\033[0m";
                cout << "\t" << "\033[1;33mWARNING:: Execuation of the command escaped. The Gmssi command \'" << this->UserCommandList.at(j) << "\'" << "could not be found" << " \033[0m\n" ; 
            }
        }
    } 

    // ofstream AgainMainFile(mainFile,ios::app);  
    // AgainMainFile << "\n" <<"include \"" << this->geometryFile << "\";\n";
    // AgainMainFile << "\n" <<"new loading stage \"" << "Stage_1 Loading" <<"\";\n";
    // AgainMainFile << "\n" <<"include \"" << this->loadFile << "\";\n";
    // AgainMainFile.close();

    int newPhysicalGroupTag =-1;NodeElement newNodeElement;
    string PhysicDes = "-1 "+ to_string(newPhysicalGroupTag)+  " \"$PythonScript$\"";
    this->PhysicalGroupMap.insert(pair<int,NodeElement>(newPhysicalGroupTag,newNodeElement));
    PhysicalGroup newPhysicalGroup = PhysicalGroup(PhysicDes);
    this->PhysicalGroupList.push_back(newPhysicalGroup); 
    PhytonScriptPhysicalGroupIndex = this->PhysicalGroupList.size()-1;

    return;
}


void gmESSITranslator::DisplayNewTagNumbering(){

    map<string,int>::iterator EssiTagIterBegin = this->EssiTagVariableMap.begin();
    map<string,int>::iterator EssiTagIterEnd = this->EssiTagVariableMap.end();
    
    cout <<endl << endl<< "\033[1;36m************************ Updated New Tag Numberring *********************" << "\033[0m\n";
    for(map<string,int>::iterator it = EssiTagIterBegin ; it!= EssiTagIterEnd ; ++it){

        cout << "\033[1;36m" << setw(15) << it->first << " = " << it->second << "\033[0m\n";
    }
    return;
}

void gmESSITranslator::AddNodeCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile,ios::app); int init=0;int nofRun=0;
    GeometryFile << PrintStartConversion(j);

    if(!(this->UserCommandList.at(j).substr(4,3).compare("All"))){

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
    setTypeIter(TypeIter,this->VariableList.at(j),i,j,init);
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

        if(n<Variables.size()){
            
            string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ; 
            this->clear(this->TempVariable);
            throw msg.c_str();
        }

        GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    GeometryFile << PrintEndConversion(nofRun,j);
    GeometryFile.close();
    return;
}

void gmESSITranslator::ElementalCommand(const int& i, const int& j){
  
    ofstream GeometryFile(geometryFile, ios::app); int init=0;
    GeometryFile << PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j),i,j,init);
    /// Ends Initialization here

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
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

            if(n<Variables.size()){
            
                string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ;
                this->clear(this->TempVariable); 
                throw msg.c_str();
            }

            GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
        }
    }

    GeometryFile << PrintEndConversion(nofRun,j);
    GeometryFile.close();
    return;
}

void gmESSITranslator::ElementalCompoundCommand(const int& i, const int& j){

    ofstream LoadFile(loadFile,ios::app); int init=0;
    LoadFile<< PrintStartConversion(j);

    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
    int VariablesByUser=Variables.size(), VariablesToFind=0;

    for(int l=0 ; l<NofEssiVariables ;l++ ){

        Tokenizer temptknzr = Tokenizer(this->FunctionIter->second.getVarList().at(l),"#");
        if(temptknzr.countTokens()==2)
            VariablesToFind++;
    }

    // cout << NofEssiVariables << "  " << VariablesToFind << "  " << VariablesByUser << endl;
    map<int,NodeElement>::iterator TypeIter;
    map<int,NodeElement>::iterator SurfaceIter;

    if(VariablesToFind+VariablesByUser > NofEssiVariables+1){
        setTypeIter(TypeIter,this->VariableList.at(j),i,j,init); 
        setTypeIter(SurfaceIter,this->VariableList.at(j).at(1));
    }
    else{
        TypeIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
        setTypeIter(SurfaceIter,this->VariableList.at(j).at(0));
    }

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    int ElementListSize = ElementList.size();

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
                         
                        if(SurfaceIter==this->PhysicalGroupMap.end()||SurfaceIter==this->EntityMap.end()){

                            string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no such Physical/Entity Group" + " \033[0m\n";
                            throw msg.c_str(); 
                        }

                        map<int,int>::iterator NodeIter = SurfaceIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));
                        map<int,int>::iterator NodeIterEnd = SurfaceIter->second.NodeList.end();

                        // if(NodeIter==SurfaceIter->second.NodeList.end()){

                        //     string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
                        //     cout <<  msg;   
                        // }

                        if(NodeIter!= NodeIterEnd){
                            this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
                            loop = false;
                        }

                        m++;
                    
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

            if(n<Variables.size()){
                string msg = "\033[1;31mERROR:: The command1 \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ; 
                this->clear(this->TempVariable);
                throw msg.c_str();
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
    return;
}

void gmESSITranslator::NodalCommand(const int& i, const int& j){

    ofstream LoadFile(loadFile,ios::app); int init =0;
    LoadFile<< PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j),i,j,init);
    /// Ends Initialization here

    map<int,int> NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
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

        if(n<Variables.size()){

            string msg = "\033[1;31mERROR:: The command1 \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ; 
            this->clear(this->TempVariable);
            throw msg.c_str();
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    LoadFile << PrintEndConversion(nofRun,j);
    LoadFile.close();
    return;
}

void gmESSITranslator::GeneralElementalCommand(const int& i, const int& j){

    ofstream LoadFile(loadFile,ios::app); int init=0;
    LoadFile<< PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j),i,j,init);
    /// Ends Initialization here

    vector<Element> ElementList =TypeIter->second.ElementList;
    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
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

        if(n<Variables.size()){
            
            string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ;
            this->clear(this->TempVariable);
            throw msg.c_str();
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    LoadFile << PrintEndConversion(nofRun,j);
    LoadFile.close();
    return;
}

void gmESSITranslator::SingularCommand(const int& i, const int& j){

    ofstream MainFile(mainFile,ios::app);

    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
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
    return;
}

void gmESSITranslator::ConnectCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile,ios::app);
    vector<string> Variables = VariableList.at(j);
    int newPhysicalGroupTag = 0;
    if(PhytonScriptPhysicalGroupIndex==-1) newPhysicalGroupTag = this->PhysicalGroupMap.size()+1;
    else newPhysicalGroupTag = this->PhysicalGroupMap.size();

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator Iterator1;
    setTypeIter(Iterator1,Variables.at(0));
    /// Ends Initialization here
    map<int,int>::iterator Iterator1Begin = Iterator1->second.NodeList.begin();
    map<int,int>::iterator Iterator1End = Iterator1->second.NodeList.end();

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator Iterator2;
    setTypeIter(Iterator2,Variables.at(1));
    /// Ends Initialization here
    map<int,int>::iterator Iterator2Begin = Iterator2->second.NodeList.begin();
    map<int,int>::iterator Iterator2End = Iterator2->second.NodeList.end();

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator Iterator3;
    setTypeIter(Iterator3,Variables.at(2));
    /// Ends Initialization here
    map<int,int>::iterator Iterator3Begin = Iterator3->second.NodeList.begin();
    map<int,int>::iterator Iterator3End = Iterator3->second.NodeList.end();

    string vector = (this->delSpaces(Variables.at(3))); Tokenizer tkr = Tokenizer(vector,"\\");
    if(tkr.countTokens()!=3) {
        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Direction Vector. It should be as x_comp\\y_comp\\z_comp" + " \033[0m\n" ; 
        throw msg.c_str();
    }

    double vec_x = stof(trim(tkr.nextToken())),vec_y = stof(trim(tkr.nextToken())),vec_z = stof(trim(tkr.nextToken()));double sum = vec_x*vec_x+vec_y*vec_y+vec_z*vec_z;
    vec_x=vec_x/sqrt(sum);vec_y=vec_y/sqrt(sum);vec_z=vec_z/sqrt(sum);
    double length = stof(this->delSpaces(Variables.at(4))); // Magnitude of vector
    int NofLayers = stoi(this->delSpaces(Variables.at(5))); // Number of layers
    string algo = this->delSpaces(Variables.at(6));      // Algo 
    double tolerence = stof(this->delSpaces(Variables.at(7))); // Tolerence 
    string newPhysicalGroupName = this->delSpaces(Variables.at(8)); //new PhysicalGroupName specified by the usrer

    if(Iterator1==this->EntityMap.end()||Iterator2==this->EntityMap.end()||(Iterator3==this->EntityMap.end() && algo!=("find"))||Iterator1==this->PhysicalGroupMap.end()||Iterator2==this->PhysicalGroupMap.end()||(Iterator3==this->PhysicalGroupMap.end() && algo!=("find"))){

        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no one of Physical/Entity Group" + " \033[0m\n";
        throw msg.c_str(); 
    }

    map<int,Node> ::iterator NodeMap1, NodeMap2;
    struct NodeElement newNodeElement;

    int Node1=0,Node2=0,EntityNo = this->NewEntity++;
    int NofElementsCreated=0 , NofNodesCreated = 0;

    for(map<int,int>::iterator It1 = Iterator1Begin; It1!=Iterator1End ;++It1){

        NodeMap1 = this->NodeMap.find(It1->second);
        NofNodesCreated++;

        for(int i=1 ; i<NofLayers; i++){

            double node1_x = NodeMap1->second.getXcord()+length*(vec_x);
            double node1_y = NodeMap1->second.getYcord()+length*(vec_y);
            double node1_z = NodeMap1->second.getZcord()+length*(vec_z);

            if(!algo.compare("create")){
        
                string str = "node";
                Node newNode = Node(stoi(getVariable(str)), node1_x, node1_y, node1_z );
                this->NodeMap.insert(pair<int,Node>(newNode.getId(),newNode)); str = "element";
                Node1=NodeMap1->first;
                Node2=newNode.getId(); 
                NofNodesCreated++;

                // cout << endl << newNode.getId() << " " << node1_x << " " << node1_y << " " << node1_z;
                // cout << " New Nodes and Elements Created" << endl;
                NofElementsCreated+=1; 
                string ElementDesc = this->getVariable(str)+ " 1 2 "  + to_string(newPhysicalGroupTag) + " " + to_string(EntityNo) + " " + to_string(Node1) + " " + to_string(Node2);
                // cout << endl << ElementDesc;
                Element newElement = Element(ElementDesc);
                this->GmshParse.addElement(newElement);
                newNodeElement.ElementList.push_back(newElement);
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                NodeMap1 = this->NodeMap.find(newNode.getId());
            }
            else if (!algo.compare("find")){
                
                int UniqueNodes = 0;

                for(map<int,int>::iterator It3 = Iterator3Begin; It3!=Iterator3End ;++It3){

                    NodeMap2 = this->NodeMap.find(It3->second);

                    double modx = abs(node1_x-NodeMap2->second.getXcord());
                    double mody = abs(node1_y-NodeMap2->second.getYcord());
                    double modz = abs(node1_z-NodeMap2->second.getZcord());

                    if(sqrt(modx*modx+ mody*mody +modz*modz)<=tolerence){

                        Node1=NodeMap1->first;
                        Node2=It3->second; string str = "element";
                        NofNodesCreated++;
                        NofElementsCreated+=1;
                        // cout << "New Elements Created from finding nodes" << endl; 
                        string ElementDesc = this->getVariable(str)+ " 1 2 "  + to_string(newPhysicalGroupTag) + " " + to_string(EntityNo) + " " + to_string(Node1) + " " + to_string(Node2);
                        // cout << endl << ElementDesc;
                        Element newElement = Element(ElementDesc);
                        this->GmshParse.addElement(newElement);
                        newNodeElement.ElementList.push_back(newElement);
                        newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                        newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                        UniqueNodes++;
                        
                        if(UniqueNodes>1){ string str =  "\033[1;31mERROR:: More than one node inside tolerence \033[0m\n"; throw str.c_str();}
                    }
                }
                NodeMap1 = NodeMap2;
            }
            else {

                string str =  "\033[1;31mERROR:: Please enter algo as create or find \033[0m\n";
                throw str.c_str();
                exit(EXIT_FAILURE);
            }
        }

        double node1_x = NodeMap1->second.getXcord()+length*(vec_x);
        double node1_y = NodeMap1->second.getYcord()+length*(vec_y);
        double node1_z = NodeMap1->second.getZcord()+length*(vec_z);
        int UniqueNodes = 0;


        for(map<int,int>::iterator It2 = Iterator2Begin; It2!=Iterator2End ;++It2){

            NodeMap2 = this->NodeMap.find(It2->second);
            double modx = abs(node1_x-NodeMap2->second.getXcord());
            double mody = abs(node1_y-NodeMap2->second.getYcord());
            double modz = abs(node1_z-NodeMap2->second.getZcord());

            if(sqrt(modx*modx+ mody*mody +modz*modz)<=tolerence){
                    
                Node1=NodeMap1->first; NofElementsCreated+=1;NofNodesCreated++;                   
                Node2=It2->second; string str = "element";
                // cout << "New Elements Created" << endl; 
                string ElementDesc = this->getVariable(str) + " 1 2 "  + to_string(newPhysicalGroupTag) + " " + to_string(EntityNo)  + " " + to_string(Node1) + " " + to_string(Node2);
                // cout << endl << ElementDesc ;
                Element newElement = Element(ElementDesc);
                this->GmshParse.addElement(newElement);
                newNodeElement.ElementList.push_back(newElement);
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node2));
                UniqueNodes++;
                
                if(UniqueNodes>1){ string str =  "\033[1;31mERROR:: More than one node inside tolerence \033[0m\n"; throw str.c_str();}
            }
        }
    }

    cout << "Sucessfully Converted" << endl;

    if (NofElementsCreated ==0){

        cout << "\033[1;33mWarning:: No elements were created \033[0m\n";
        return;
    }

    this->PhysicalGroupMap.insert(pair<int,NodeElement>(newPhysicalGroupTag,newNodeElement));
    string PhysicDes = "1 "+ to_string(newPhysicalGroupTag)+  " \"$" + newPhysicalGroupName +  "$\""; 
    PhysicalGroup newPhysicalGroup = PhysicalGroup(PhysicDes);
    this->PhysicalGroupList.push_back(newPhysicalGroup);
    this->PhysicalStringNameToIdMap.insert(pair<string,int>(newPhysicalGroup.getPhysicTag(),newPhysicalGroup.getId()));
    this->PhysicalStringNameToIdMap.insert(pair<string,int>(to_string(newPhysicalGroup.getId()),newPhysicalGroup.getId()));

    cout << "\033[1;36mNew Physical Group \"" << newPhysicalGroupName << "\" having id " << newPhysicalGroupTag << " consisting of " << NofNodesCreated <<" Nodes and " << NofElementsCreated << " 2-noded elements created "  << "\033[0m\n";
    GeometryFile.close();
    return;
}

void gmESSITranslator::MaterialVariationalCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile, ios::app); int init =1;
    ofstream MainFile(mainFile, ios::app);
    GeometryFile << PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    setTypeIter(TypeIter,this->VariableList.at(j),i,j,init);
    /// Ends Initialization here

    map<string,int>::iterator MaterialTagIter;
    map<string,Semantics>::iterator TempFunctionIter;

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
  
    Tokenizer tknzr = Tokenizer(Variables.at(init-1),"()");
    string gmssiCommandtag, gmssiArguments;

    try{
        if(tknzr.countTokens()!=2) throw exception();
        gmssiCommandtag =tknzr.nextToken()+"{";
        gmssiArguments = tknzr.nextToken()+"}";
    } catch (exception& e) { string str = "\033[1;31mERROR:: Syntax Error in " +  Variables.at(init-1) + " \033[0m\n" ; throw str.c_str(); return;}

    replace( gmssiArguments.begin(), gmssiArguments.end(), ';', ',' );
    tknzr.set(gmssiArguments,",");
    if(tknzr.countTokens()>1) gmssiArguments=","+gmssiArguments;

    string ElementalCommand =  gmssiCommandtag +"1"+ gmssiArguments;
    PhysicalGroup TempPhyGroup = PhysicalGroup(); TempPhyGroup.Process(ElementalCommand);
    TempFunctionIter = this->FunctionMap.find(TempPhyGroup.getCommandList().at(0));

    if (TempFunctionIter == this->FunctionMap.end()){
        string str =  "\033[1;31mError:: The essi command \'" + Variables.at(init-1) + "\'" + "could not be found" + " \033[0m\n" ;
        throw str.c_str();
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

            try{

                for(int l=0 ; l<NofEssiVariables ;l++ ){

                    if (!EssiVariables.at(l).compare("material#1")){
                       string ScriptFunction = "material";
                       this->TempVariable.push(this->getVariable(ScriptFunction));
                    }
                    else{
                        string UserVariable = Variables.at(n++), unit="", prec="0", value, function;
                        Tokenizer tknzr = Tokenizer(UserVariable,"\\");
                        string ScriptFunction = tknzr.nextToken();
                        function = ScriptVariables + ScriptFunction + ";";
                        value = this->Evaluate.eval(function);
                        if(tknzr.hasMoreTokens()){
                            string temp = delSpaces(tknzr.nextToken()); 
                            if (temp.compare("")) prec = temp ;
                            value = to_string(roundToSignificantFigures(stof(this->Evaluate.eval(function)), stoi(prec)));
                        }
                        if(tknzr.hasMoreTokens()){ unit = unit + "*" + delSpaces(tknzr.nextToken()); }

                        value = value + unit;          
                        Material = Material + " " + value;
                        this->TempVariable.push(value);  
                    }
                }

            } catch (exception& e) { string str = "\033[1;31mERROR:: Syntax Error " +  Variables.at(n-1) + " \033[0m\n" ; throw str.c_str(); exit(EXIT_FAILURE);}

            if(n<Variables.size()){
            
                string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has syntaxERROR in Phy/Enty# tag" + " \033[0m\n" ;
                this->clear(this->TempVariable);
                throw msg.c_str();
            }

            MaterialTagIter = this->MaterialTag.find(Material);

            if(MaterialTagIter!=this->MaterialTag.end()){
                MatTag = MaterialTagIter->second;
                clear( this->TempVariable );
                this->EssiTagVariableMap.find("material")->second = this->EssiTagVariableMap.find("material")->second-1;
            }
            else{
                MainFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
                MatTag = this->EssiTagVariableMap.find("material")->second-1;
                this->MaterialTag.insert(pair<string,int>(Material,MatTag));
            }

            ElementalCommand = gmssiCommandtag +to_string(MatTag)+ gmssiArguments;
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
                }
                else{string newVar = NewVariables.at(n++);
                    this->TempVariable.push(newVar);

                    /****************************************************** Updating Essi Tag *********************************************************/
                    Tokenizer temptknzr = Tokenizer(TempFunctionIter->second.getEssiVarList().at(l),"#");
                    string tempvar = temptknzr.nextToken(); 
                    map<string,int>::iterator EssiTagVariableMapIter = this->EssiTagVariableMap.find(tempvar);
                    if (EssiTagVariableMapIter!= this->EssiTagVariableMap.end()){ EssiTagVariableMapIter->second = stoi(newVar)+1;}
                   /****************************************************** Updating Essi Tag *********************************************************/
                }
            }

            GeometryFile << this->PrintEssiCommand(TempFunctionIter->second.getEssiCommand(),TempFunctionIter->second.getNofEssiVariables(),j);
        }
    }

    GeometryFile << PrintEndConversion(nofRun,j);
    GeometryFile.close();
    remove( "script" );
    MainFile.close();
    return;
}

void gmESSITranslator::WriteCommand(const int&i, const int& j){

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter; int init=0;
    setTypeIter(TypeIter,this->VariableList.at(j),i,j,init);
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
        ElementsFile << ElementList.at(i).getType() << "\t";
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


string gmESSITranslator::getVariable(string& var){

    map<string,int>::iterator EssiTagIter = this->EssiTagVariableMap.find(var);

    if(!var.compare("nodes")){

        map<string,int>::iterator  FindIter = this->EssiTagVariableMap.find("node");
        FindIter->second++;
    }
    else if(!var.compare("node")){

        map<string,int>::iterator FindIter = this->EssiTagVariableMap.find("nodes");
        FindIter->second++;
    }

    return  to_string(EssiTagIter->second++);
}

string gmESSITranslator::PrintEssiCommand(string Command, int NofEssiVariables, int j){

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
        Ecommand << inpString.nextToken()  << setw(0);
        Ecommand << TempVariable.front()   << setw(0) ;
        TempVariable.pop();
    }
    
    Ecommand <<inpString.nextToken() << setw(0) <<"\n";
    return Ecommand.str();
}

void gmESSITranslator::clear( queue<string> &q ){

   queue<string> empty;
   swap( q, empty );
   return;
}

string gmESSITranslator::delSpaces(string str){

   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}

string gmESSITranslator::trim(const string& str, const string& delimiters ){
    string s=str;
    s.erase( s.find_last_not_of( delimiters ) + 1 ).erase( 0, s.erase( s.find_last_not_of( delimiters ) + 1 ).find_first_not_of( delimiters ) );
    return s;
}

double gmESSITranslator::roundToSignificantFigures(double num, int n) {
    
  double RoundedNumber =0;
  
  if(n==0)	RoundedNumber = (int) num;
  else if(n >=1)  RoundedNumber = floor((num+ 5* pow(10,-n-1))*pow(10,n))/pow(10,n);
  else RoundedNumber = floor((num+5* pow(10,-(n+2)))/pow(10,-n-1))*pow(10,-n-1);

  return RoundedNumber;
}

void gmESSITranslator::setTypeIter(map<int,NodeElement>::iterator &TypeIter,const vector<string>& variable,const int& i,const int& j, int &n){

    if(variable.size()>0){

        Tokenizer tknzr = Tokenizer((variable.at(0)),"#");
        int NofTokens = tknzr.countTokens(), tag ;

        if(NofTokens==2){

            string type = (tknzr.nextToken());

            if(!type.compare("Entity_Group")){

                try{
                    tag = stoi(tknzr.nextToken());
                } catch(exception e) {string msg = "\033[1;31mERROR:: The command  failed to convert as there is no such  Entity Group  \"" + variable.at(0)  + "\" \033[0m\n" ; throw msg.c_str();    }
                
                TypeIter = this->EntityMap.find(tag);

                if(TypeIter==this->EntityMap.end()|| TypeIter->second.NodeList.size()==0){
                    string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Entity Group \"" + variable.at(0) + "\" \033[0m\n" ; 
                    cout << msg;     
                }
            }

            else if (!type.compare("Physical_Group")){

                map<string,int>::iterator PhysicalStringNameToIdMapIter;
                PhysicalStringNameToIdMapIter = this->PhysicalStringNameToIdMap.find(tknzr.nextToken());

                if(PhysicalStringNameToIdMapIter==this->PhysicalStringNameToIdMap.end()){
                    string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no such Physical Group \"" + variable.at(0) + "\" \033[0m\n" ; 
                    throw msg.c_str();    
                }

                tag = PhysicalStringNameToIdMapIter->second;
                TypeIter = this->PhysicalGroupMap.find(tag);

                if(TypeIter->second.NodeList.size()==0){
                    string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group \"" + variable.at(0) + "\" \033[0m\n" ; 
                    cout << msg;     
                }
            }

            else{

                string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has a syntaxERROR in Entity_Group/Physical_Group#Tag tag \"" + variable.at(0) + "\" \033[0m\n" ;
                cout << msg ; exit (EXIT_FAILURE);
                throw msg.c_str();
            }

            n=n+1;
        }

        else if (NofTokens==1){

            TypeIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

            if(TypeIter==this->PhysicalGroupMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group \033[0m\n" ; 
                cout << msg;     
            }

        }
        else{

            string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has a syntaxERROR in Entity_Group/Physical_Group#Tag tag \033[0m\n" ;
            cout << msg ; exit (EXIT_FAILURE);
            throw msg.c_str();
        }

    }

    else{

        TypeIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

        if(TypeIter==this->PhysicalGroupMap.end()|| TypeIter->second.NodeList.size()==0){
            string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\' failed to convert as there is no elements/nodes in the Physical Group  \033[0m\n" ; 
            cout << msg;      
        }
    }

    return;
}

void gmESSITranslator::setTypeIter(map<int,NodeElement>::iterator &TypeIter,const string& variable){

    Tokenizer tknzr = Tokenizer((variable),"#");
    int NofTokens = tknzr.countTokens(),tag;

    if(NofTokens==2){

        string type = (tknzr.nextToken());

        if(!type.compare("Entity_Group")){

            try{
                tag = stoi(tknzr.nextToken());
            } catch(exception e) {string msg = "\033[1;31mERROR:: The command  failed to convert as there s no such  Entity Group  \"" + variable + "\" \033[0m\n" ; throw msg.c_str();    }

            TypeIter = this->EntityMap.find(tag);

            if(TypeIter==this->EntityMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command  failed to convert as there is no elements/nodes in the Entity Group  \"" + variable + "\" \033[0m\n" ; 
                cout << msg;    
            }
        }

        else if (!type.compare("Physical_Group")){

            map<string,int>::iterator PhysicalStringNameToIdMapIter;
            PhysicalStringNameToIdMapIter = this->PhysicalStringNameToIdMap.find(tknzr.nextToken());

            if(PhysicalStringNameToIdMapIter==this->PhysicalStringNameToIdMap.end()){
                string msg = "\033[1;33mERROR:: The command  failed to convert as there is no such Physical Group \"" + variable + "\" \033[0m\n" ; 
                throw msg.c_str();     
            }

            tag = PhysicalStringNameToIdMapIter->second;
            TypeIter = this->PhysicalGroupMap.find(tag);

            if(TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command  failed to convert as there is no elements/nodes in the Physical Group \"" + variable + "\" \033[0m\n" ; 
                cout << msg;     
            }
        }

        else{

            string msg = "\033[1;31mERROR:: The command  has a syntaxERROR in " +  variable  + " \033[0m\n" ;
            throw msg.c_str();
        }
    }

    else{

        string msg = "\033[1;31mERROR:: The command  has a syntaxERROR in " +  variable  + " \033[0m\n" ;
        throw msg.c_str();
    }

    return;
}

string gmESSITranslator::PrintStartConversion(const int& j){

    string str = "\n//*************************************************************************************************************************\n";
    str = str + "//\t\t\t\t\t\t\t" +  this->UserCommandList.at(j) + "Begins\n";
    str = str + "//*************************************************************************************************************************\n\n" ;
    return str;
}

string gmESSITranslator::PrintEndConversion(const int& nofRun, const int& j){

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

void gmESSITranslator::UpdateEssiTags(const string& newVar, const int& l){

    string tempvar;
    try{
        Tokenizer temptknzr = Tokenizer(this->FunctionIter->second.getEssiVarList().at(l),"#");
        tempvar = temptknzr.nextToken(); 
        map<string,int>::iterator EssiTagVariablemapIter = this->EssiTagVariableMap.find(tempvar);
        if (EssiTagVariablemapIter!= this->EssiTagVariableMap.end()) EssiTagVariablemapIter->second = boost::lexical_cast<int>(newVar)+1;

    } catch(exception& e) { string str = "\033[1;31mERROR:: Syntax Error in The command. The parameter "+ tempvar +"_no should be a integer \033[0m\n" ; throw str.c_str();}

    return;
}

void gmESSITranslator::Convert(const string& GmssiCommand){

    int i = PhytonScriptPhysicalGroupIndex;
    PhysicalGroupList.at(i).Process(GmssiCommand);

    this->CommandList = this->PhysicalGroupList.at(i).getCommandList();
    this->VariableList = this->PhysicalGroupList.at(i).getVariableList();
    this->NofVariablesList = this->PhysicalGroupList.at(i).getNofVariables();
    this->UserCommandList = this->PhysicalGroupList.at(i).getUserCommandList();
    int j = CommandList.size()-1;     
    this->FunctionIter = this->FunctionMap.find(this->CommandList.at(j));

    cout << left << setw(40) << this->UserCommandList.at(j) ;

    if (this->FunctionIter != this->FunctionMap.end()){
        
        cout<< left << setw(15) << "\t \t Found!!";

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

    return;
}

void gmESSITranslator::UpdateGmshFile(){

	string UpdatedGmshFile = this->pwd +".msh";
    ofstream UpdateGmshFile(UpdatedGmshFile,ios::out); 

    int PhysicalGroupListSize = this->PhysicalGroupList.size()-1;

    UpdateGmshFile << "$MeshFormat\n2.2 0 8\n$EndMeshFormat\n" << "$PhysicalNames\n" << PhysicalGroupListSize << "\n";

    for (int i=0; i<PhysicalGroupListSize+1; i++)
        if(this->PhysicalGroupList.at(i).getId()>0)
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

    return;
}