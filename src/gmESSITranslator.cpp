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
#include <boost/regex.hpp>

using namespace::std;

int GMSH_to_ESSI_NODE_CONNECTIVITY[18][28];

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

gmESSITranslator::gmESSITranslator(){}

gmESSITranslator::gmESSITranslator(const string& gmshFile, const string& newDir){

    GmshFile = gmshFile;
    ESSI_Simulation_Dir = newDir;
    Tokenizer tknzr = Tokenizer(gmshFile,"/ ."); 
    tknzr.setcurrPos(gmshFile.length()-1);tknzr.setMode(1);tknzr.nextToken();tknzr.nextToken();
    this->pwd = newDir+tknzr.currToken(); 
    geometryFile = newDir + tknzr.currToken() + "_geometry.fei";
    loadFile = newDir + tknzr.currToken() + "_load.fei";
    mainFile = newDir + tknzr.currToken() + "_analysis.fei";
}

gmESSITranslator::gmESSITranslator(const string& gmshFile, const string& mappingFile, const string& newDir){

    GmshFile = gmshFile;
    ESSI_Simulation_Dir = newDir;
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
    ESSI_Simulation_Dir = newDir;
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

    this->Generate_Make_File();

    this->GMSH_to_ESSI_NODE_Mapping();
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

    this->EssiTagVariableMap.insert(pair<string,int>("element",1));
    // this->EssiTagVariableMap.insert(pair<string,int>("node",this->GmshParse.getNewNode()));
    // this->EssiTagVariableMap.insert(pair<string,int>("nodes",this->GmshParse.getNewNode()));
    this->EssiTagVariableMap.insert(pair<string,int>("node",1));
    this->EssiTagVariableMap.insert(pair<string,int>("nodes",1));
    this->EssiTagVariableMap.insert(pair<string,int>("damping",1));
    this->EssiTagVariableMap.insert(pair<string,int>("displacement",1));
    this->EssiTagVariableMap.insert(pair<string,int>("field",1));
    this->EssiTagVariableMap.insert(pair<string,int>("load",1));
    this->EssiTagVariableMap.insert(pair<string,int>("material",1));
    this->EssiTagVariableMap.insert(pair<string,int>("motion",1));

    /***************ElementNodemap assigning to 0***********************/
    vector<Element> ElementList= this->GmshParse.getElementList();
    int ElementListSize = ElementList.size();
    for (int i =0 ; i < ElementListSize ; i++)
    	this->ElementNoMap.insert(pair<int,int>(ElementList.at(i).getId(),0));
	/*******************************************************************/

    /***************NodeNodemap assigning to 0***********************/
    // int NodeListSize = this->NodeMap.size();
    // for (int i =1 ; i <= NodeListSize ; i++)
    //     this->NodeNoMap.insert(pair<int,int>(NodeMap,0));

    for (map<int,Node>::iterator it=NodeMap.begin(); it!=NodeMap.end(); ++it)
        this->NodeNoMap.insert(pair<int,int>(it->second.getId(),0));

    /*******************************************************************/
    int PhysicalGroupListSize = this->PhysicalGroupList.size();

    for (int i=0; i<PhysicalGroupListSize; i++){

        this->CommandList = this->PhysicalGroupList.at(i).getCommandList();
        this->VariableList = this->PhysicalGroupList.at(i).getVariableList();
        this->NofVariablesList = this->PhysicalGroupList.at(i).getNofVariables();
        this->UserCommandList = this->PhysicalGroupList.at(i).getUserCommandList();
        int CommandListSize = CommandList.size();

        for (int j=0; j<CommandListSize; j++){
            
            this->FunctionIter = this->FunctionMap.find(this->CommandList.at(j));
            cout << "\n" << this->UserCommandList.at(j) ;

            if (this->FunctionIter != this->FunctionMap.end()){
                
                cout<< left << setw(15) << "\t \t Found!!";

                if(this->FunctionIter->second.getMatchMode() && !(this->FunctionIter->second.getSemanticsId().compare("c")))
                    this->ElementalCompoundCommand(i,j);
                else if(this->FunctionIter->second.getMatchMode() && !(this->FunctionIter->second.getSemanticsId().compare("vc")))
                    this->ElementalCompoundVariationalCommand(i,j);
                else if(this->FunctionIter->second.getMatchMode() && !(this->FunctionIter->second.getSemanticsId().compare("v")))
                    this->ElementalVariationalCommand(i,j);
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
                else if (!this->FunctionIter->second.getElementId().compare("nv"))
                    this->NodalVariationalCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("ev"))
                    this->GeneralElementalVariationalCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("wr"))
                    this->WriteCommand(i,j);
            }
            else{
                
                cout << "\n \t \033[1;31mNot Found!!" << "\033[0m";
                cout << "\n \t \033[1;33mWARNING:: Execuation of the command escaped. The gmessi command could not be found" << " \033[0m\n\n" ; 
            }
        }
    } 

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

    // /***************ElementNodemap assigning to 0***********************/
    // map<int,int>::iterator ElementNoMapBegin = this->ElementNoMap.begin();
    // map<int,int>::iterator ElementNoMapEnd = this->ElementNoMap.end();
    // int noe =0; int non=0;
    // for(map<int,int>::iterator it = ElementNoMapBegin ; it!= ElementNoMapEnd ; ++it)
    //     if (it->second!=0) noe=noe+1;
    // *****************************************************************

    // /***************NodeNodemap assigning to 0***********************/
    // map<int,int>::iterator NodeNoMapBegin = this->NodeNoMap.begin();
    // map<int,int>::iterator NodeNoMapEnd = this->NodeNoMap.end();
    // for(map<int,int>::iterator it = NodeNoMapBegin ; it!= NodeNoMapEnd ; ++it)
    //     if (it->second!=0) non=non+1;
    // /*******************************************************************/
    
    cout <<endl << endl<< "\033[1;36m************************ Updated New Tag Numbering **********************" << "\033[0m\n";
    for(map<string,int>::iterator it = EssiTagIterBegin ; it!= EssiTagIterEnd ; ++it){

        cout << left << "\033[1;36m" << setw(15) << it->first << " = " << it->second << "\033[0m\n";
    }

    // cout << "\n\n--------------";
    // cout << "\n--------------\n";
    cout << left << "\033[1;36m" << setw(15) << "Gmsh_Elements" << " = " << this->GmshParse.getElementList().size()+1 << "\033[0m\n";
    cout << left << "\033[1;36m" << setw(15) << "Gmsh_Nodes" << " = " << this->NodeMap.size()+1 << "\033[0m\n";
    return;
}

void gmESSITranslator::AddNodeCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile,ios::app); int init=0;int nofRun=0;
    GeometryFile << PrintStartConversion(j);  string node = "node";

    if(!(this->UserCommandList.at(j).substr(4,3).compare("All"))){

        map<int,Node> ::iterator AllNodeBegin = this->NodeMap.begin();
        map<int,Node> ::iterator AllNodeEnd = this->NodeMap.end();

        for(map<int,Node>::iterator it=AllNodeBegin; it!=AllNodeEnd; ++it){

            /******************************** OPtimizing Nodes for ESSI *****************************************/
            if(this->NodeNoMap.find(it->second.getId())->second==0){
                string NewNodeNo = this->getVariable(node);
                this->TempVariable.push(NewNodeNo);
                this->NodeNoMap.find(it->second.getId())->second = stoi(NewNodeNo);nofRun++;
            }
            else{
                this->TempVariable.push(to_string(this->NodeNoMap.find(it->second.getId())->second));nofRun++;
            }
            /****************************************************************************************************/
        
            // this->TempVariable.push(to_string(it->second.getId()));nofRun++;
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
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
    /// Ends Initialization here

    map<int,int> NodeList = TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();

    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){
        
        int n = init; nofRun++;
        map<int,Node>::iterator NodeInfo = this->NodeMap.find(it->second);

        /******************************** OPtimizing Nodes for ESSI *****************************************/
        if(this->NodeNoMap.find(it->second)->second==0){
            string NewNodeNo = this->getVariable(node);
            this->TempVariable.push(NewNodeNo);
            this->NodeNoMap.find(it->second)->second = stoi(NewNodeNo);
        }
        else{
            this->TempVariable.push(to_string(this->NodeNoMap.find(it->second)->second));
        }
        /****************************************************************************************************/

        // this->TempVariable.push(to_string(it->second));
        this->TempVariable.push(to_string(NodeInfo->second.getXcord())+"*"+this->VariableList.at(j).at(n)); 
        this->TempVariable.push(to_string(NodeInfo->second.getYcord())+"*"+this->VariableList.at(j).at(n));
        this->TempVariable.push(to_string(NodeInfo->second.getZcord())+"*"+this->VariableList.at(j).at(n)); 
        this->TempVariable.push(Variables.at(n+1)); 

        // if(n<Variables.size()){
            
        //     string msg = "\n \t \033[1;31mERROR:: The command has syntaxERROR in Phy/Enty# tag\033[0m\n" ; 
        //     this->clear(this->TempVariable);
        //     throw msg.c_str();
        // }

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
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
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

                    /******************************** OPtimizing Elements for ESSI *****************************************/
                    if(this->ElementNoMap.find(ElementList.at(k).getId())->second==0){
                    	string NewElementNo = this->getVariable(var);
                    	this->TempVariable.push(NewElementNo);
                    	this->ElementNoMap.find(ElementList.at(k).getId())->second = stoi(NewElementNo);
                    }
                    else
                    	this->TempVariable.push(to_string(this->ElementNoMap.find(ElementList.at(k).getId())->second));
                    /******************************************************************************************************/
                }
                else if(!var.compare("node") || !var.compare("nodes")){

                    /******************************** OPtimizing for ESSI *****************************************/
                    m=m+1;if(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second==0){
                        string NewNodeNo = this->getVariable(var);
                        this->TempVariable.push(NewNodeNo);
                        this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second = stoi(NewNodeNo);
                    }
                    else
                        this->TempVariable.push(to_string(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second));
                    /*********************************************************************************************/

                    // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
                    // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][++m]-1)));                
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
            
                string msg = "\n \t \033[1;31mERROR:: The command has syntaxERROR in Phy/Enty# tag \033[0m\n" ;
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

    map<int,NodeElement>::iterator TypeIter;
    map<int,NodeElement>::iterator SurfaceIter;

    if(VariablesToFind+VariablesByUser > NofEssiVariables+1){
        if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
        if( setTypeIter(SurfaceIter,this->VariableList.at(j).at(1))<0)
            return;
    }
    else{
        TypeIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
        if( setTypeIter(SurfaceIter,this->VariableList.at(j).at(0))<0)
            return;
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
                    /******************************** OPtimizing Elements for ESSI *****************************************/
                    if(this->ElementNoMap.find(ElementList.at(k).getId())->second==0){
                    	string NewElementNo = this->getVariable(var);
                    	this->TempVariable.push(NewElementNo);
                    	this->ElementNoMap.find(ElementList.at(k).getId())->second = stoi(NewElementNo);
                    }
                    else
                    	this->TempVariable.push(to_string(this->ElementNoMap.find(ElementList.at(k).getId())->second));
                    /******************************************************************************************************/
                }
                else if(!var.compare("node") || !var.compare("nodes")){

                    bool loop=true; int loopMax =  ElementList.at(k).getNodeList().size();

                    while(loop && m<loopMax){
                         
                        if(SurfaceIter==this->PhysicalGroupMap.end()||SurfaceIter==this->EntityMap.end()){

                            string msg = "\n \t \033[1;31mERROR:: The command failed to convert as there is no such Physical/Entity Group\033[0m\n";
                            throw msg.c_str(); 
                        }

                        map<int,int>::iterator NodeIter = SurfaceIter->second.NodeList.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m+1]-1));

                        // map<int,int>::iterator NodeIter = SurfaceIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));
                        map<int,int>::iterator NodeIterEnd = SurfaceIter->second.NodeList.end();

                        // if(NodeIter==SurfaceIter->second.NodeList.end()){

                        //     string msg = "\n \t \033[1;33mWARNING:: The command could not find any nodes/elements on which it operates\033[0m\n" ; 
                        //     cout <<  msg;   
                        // }

                        if(NodeIter!= NodeIterEnd){

                            /******************************** OPtimizing Nodes for ESSI *****************************************/
                            if(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m+1]-1))->second==0){
                                string NewNodeNo = this->getVariable(var);
                                this->TempVariable.push(NewNodeNo);
                                this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m+1]-1))->second = stoi(NewNodeNo);
                            }
                            else
                                this->TempVariable.push(to_string(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m+1]-1))->second));
                            /*********************************************************************************************/
                            // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
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
                string msg = "\n \t \033[1;31mERROR:: The command1 has syntaxERROR in Phy/Enty# tag\033[0m\n" ; 
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
                    if(!var.compare("node") || !var.compare("nodes") || !var.compare("element")) continue;
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
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
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

                /******************************** OPtimizing Nodes for ESSI *****************************************/
                if(this->NodeNoMap.find(it->second)->second==0){
                    string NewNodeNo = this->getVariable(var);
                    this->TempVariable.push(NewNodeNo);
                    this->NodeNoMap.find(it->second)->second = stoi(NewNodeNo);
                }
                else
                    this->TempVariable.push(to_string(this->NodeNoMap.find(it->second)->second));
                /***************************************************************************************************/

                // this->TempVariable.push(to_string(it->second));  
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

            string msg = "\n \t \033[1;31mERROR:: The command has syntaxERROR in Phy/Enty# tag\033[0m\n" ; 
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
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
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

                /******************************** OPtimizing Elements for ESSI *****************************************/

                if(this->ElementNoMap.find(ElementList.at(k).getId())->second==0){
                	string NewElementNo = this->getVariable(var);
                	this->TempVariable.push(NewElementNo);
                	this->ElementNoMap.find(ElementList.at(k).getId())->second = stoi(NewElementNo);
                }
                else
                	this->TempVariable.push(to_string(this->ElementNoMap.find(ElementList.at(k).getId())->second));

                /******************************************************************************************************/
            }
            else if(!var.compare("node") || !var.compare("nodes")){

                /******************************** OPtimizing for ESSI *****************************************/
                m=m+1;if(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second==0){
                    string NewNodeNo = this->getVariable(var);
                    this->TempVariable.push(NewNodeNo);
                    this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second = stoi(NewNodeNo);
                }
                else
                    this->TempVariable.push(to_string(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second));
                /*********************************************************************************************/

                // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
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
            
            string msg = "\n \t \033[1;31mERROR:: The command has syntaxERROR in Phy/Enty# tag\033[0m\n" ;
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

    cout << "\n \t Sucessfully Converted" << endl;
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
    if( setTypeIter(Iterator1,Variables.at(0))<0)
        return;
    /// Ends Initialization here
    map<int,int>::iterator Iterator1Begin = Iterator1->second.NodeList.begin();
    map<int,int>::iterator Iterator1End = Iterator1->second.NodeList.end();

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator Iterator2;
    if( setTypeIter(Iterator2,Variables.at(1))<0)
        return;
    /// Ends Initialization here
    map<int,int>::iterator Iterator2Begin = Iterator2->second.NodeList.begin();
    map<int,int>::iterator Iterator2End = Iterator2->second.NodeList.end();

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator Iterator3;
    if( setTypeIter(Iterator3,Variables.at(2))<0)
        return;
    /// Ends Initialization here
    map<int,int>::iterator Iterator3Begin = Iterator3->second.NodeList.begin();
    map<int,int>::iterator Iterator3End = Iterator3->second.NodeList.end();

    string vector = (this->delSpaces(Variables.at(3))); Tokenizer tkr = Tokenizer(vector,"\\");
    if(tkr.countTokens()!=3) {
        string msg = "\n \t \033[1;31mERROR:: The command has syntaxERROR in Direction Vector. It should be as x_comp\\y_comp\\z_comp\033[0m\n" ; 
        throw msg.c_str();
    }

    double vec_x = 0, vec_y = 0, vec_z =0; vec_x = stof(trim(tkr.nextToken())); boost::regex  AssignRegex("[^(:=)]*:=\\s*"); boost::sregex_iterator end;
    string variable = trim(tkr.nextToken()); /*cout << variable << " " ;*/ boost::sregex_iterator it(variable.begin(), variable.end(), AssignRegex);  if(it!= end)  variable = variable.substr(it->str().length(),variable.length()); vec_y= stof(trim(variable));
    variable = trim(tkr.nextToken()); /*cout << variable << " " ;*/ boost::sregex_iterator its(variable.begin(), variable.end(), AssignRegex); if(its!= end)  variable = variable.substr(its->str().length(),variable.length()); vec_z= stof(trim((variable)));

    // cout << "***********************************************************************************" << endl;
    // cout << " " << vec_x ; cout  << " " << vec_y; cout << " " << vec_z  << endl;
    // cout << "***********************************************************************************" << endl;

    double sum = vec_x*vec_x+vec_y*vec_y+vec_z*vec_z;
    if (sum==0){vec_x=1.0/sqrt(3);vec_y=1.0/sqrt(3);vec_z=1.0/sqrt(3);}
    else { vec_x=vec_x/sqrt(sum);vec_y=vec_y/sqrt(sum);vec_z=vec_z/sqrt(sum);}

    double length = stof(this->delSpaces(Variables.at(4))); // Magnitude of vector
    int NofLayers = stoi(this->delSpaces(Variables.at(7))); // Number of layers
    string algo = this->delSpaces(Variables.at(6));      // Algo 
    double tolerence = stof(this->delSpaces(Variables.at(5))); // Tolerence 
    string newPhysicalGroupName = this->delSpaces(Variables.at(8)); //new PhysicalGroupName specified by the usrer

    // cout << "Length= " << length << " NofLayers= " << NofLayers << "algo= " << algo << " tolerence= " << tolerence << endl;
    // cout << (Variables.at(5)) << endl;

    if(Iterator1==this->EntityMap.end()||Iterator2==this->EntityMap.end()||(Iterator3==this->EntityMap.end() && algo!=("find"))||Iterator1==this->PhysicalGroupMap.end()||Iterator2==this->PhysicalGroupMap.end()||(Iterator3==this->PhysicalGroupMap.end() && algo!=("find"))){

        string msg = "\n \t \033[1;31mERROR:: The command failed to convert as there is no one of Physical/Entity Group\033[0m\n";
        throw msg.c_str(); 
    }

    map<int,Node> ::iterator NodeMap1, NodeMap2;
    struct NodeElement newNodeElement;

    int Node1=0,Node2=0,EntityNo = this->NewEntity++;
    int NofElementsCreated=0 , NofNodesCreated = 0;

    for(map<int,int>::iterator It1 = Iterator1Begin; It1!=Iterator1End ;++It1){

        NodeMap1 =this->NodeMap.find(It1->second);
        NofNodesCreated++;

        // cout << "vec_x " << vec_x << " vect_y " << vec_y << " vec_z " << vec_z << endl;
        // cout << "length " << length << endl;

        for(int i=1 ; i<NofLayers; i++){

            double node1_x = NodeMap1->second.getXcord()+length*(vec_x);
            double node1_y = NodeMap1->second.getYcord()+length*(vec_y);
            double node1_z = NodeMap1->second.getZcord()+length*(vec_z);

            // cout << "NodeMap1->second.getXcord() " << NodeMap1->second.getXcord() << endl; 
            // cout << "NodeMap1->second.getYcord() " << NodeMap1->second.getYcord() << endl; 
            // cout << "NodeMap1->second.getZcord() " << NodeMap1->second.getZcord() << endl; 


            if(!algo.compare("create")){
                
                Node newNode = Node(this->GmshParse.getNewNode(), node1_x, node1_y, node1_z );
                this->GmshParse.addNode(newNode);
                
                this->NodeMap.insert(pair<int,Node>(newNode.getId(),newNode)); string str = "element";

                // Check if the new node exists else add to NodeNoMap list
                if ( NodeNoMap.find(newNode.getId()) == NodeNoMap.end() ) {
                  this->NodeNoMap[newNode.getId()] = 0;
                }
                
                Node1=NodeMap1->first;
                Node2=newNode.getId(); 
                NofNodesCreated++;

                // cout << endl << newNode.getId() << " " << node1_x << " " << node1_y << " " << node1_z;
                // cout << " New Nodes and Elements Created" << endl;
                NofElementsCreated+=1; 
                string ElementDesc = to_string(this->GmshParse.getNewElement()) + " 1 2 "  + to_string(newPhysicalGroupTag) + " " + to_string(EntityNo) + " " + to_string(Node1) + " " + to_string(Node2);
                // cout << endl << ElementDesc;
                Element newElement = Element(ElementDesc);
                this->GmshParse.addElement(newElement);
                this->ElementNoMap.insert(pair<int,int>(newElement.getId(),0));
                newNodeElement.ElementList.push_back(newElement);
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node1));
                newNodeElement.NodeList.insert(pair<int,int>(Node2,Node2));
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
                        string ElementDesc = to_string(this->GmshParse.getElementList().size()+1) + " 1 2 "  + to_string(newPhysicalGroupTag) + " " + to_string(EntityNo) + " " + to_string(Node1) + " " + to_string(Node2);
                        // cout << endl << ElementDesc;
                        Element newElement = Element(ElementDesc);
                        this->GmshParse.addElement(newElement);
                        this->ElementNoMap.insert(pair<int,int>(newElement.getId(),0));
                        newNodeElement.ElementList.push_back(newElement);
                        newNodeElement.NodeList.insert(pair<int,int>(Node1,Node1));
                        newNodeElement.NodeList.insert(pair<int,int>(Node2,Node2));
                        UniqueNodes++;
                        NodeMap1 = NodeMap2;
                        
                        if(UniqueNodes>1){ string str =  "\n \t \033[1;31mERROR:: More than one node inside tolerence \033[0m\n"; throw str.c_str();}
                    }
                }

                // cout <<  NodeMap2->second.getXcord() << " " << NodeMap2->second.getYcord() << " " << NodeMap2->second.getZcord()<<endl;
                // NodeMap1 = NodeMap2;
                // cout <<  NodeMap1->second.getXcord() << " " << NodeMap1->second.getYcord() << " " << NodeMap1->second.getZcord()<<endl;
            }
            else {

                string str =  "\n \t \033[1;31mERROR:: Please enter algo as create or find \033[0m\n";
                throw str.c_str();
                exit(EXIT_FAILURE);
            }
        }

        double node1_x = NodeMap1->second.getXcord()+length*(vec_x);
        double node1_y = NodeMap1->second.getYcord()+length*(vec_y);
        double node1_z = NodeMap1->second.getZcord()+length*(vec_z);
        int UniqueNodes = 0;

        // cout << "I am here" << endl;

        for(map<int,int>::iterator It2 = Iterator2Begin; It2!=Iterator2End ;++It2){

            NodeMap2 = this->NodeMap.find(It2->second);
            double modx = abs(node1_x-NodeMap2->second.getXcord());
            double mody = abs(node1_y-NodeMap2->second.getYcord());
            double modz = abs(node1_z-NodeMap2->second.getZcord());

            // cout << "modx " << modx << " mody " << mody << " modz " << modz << endl;
            // cout << "Tolerence " << sqrt(modx*modx+ mody*mody +modz*modz) << endl;
            // cout << "Tolerence by user " << tolerence << endl;
            if(sqrt(modx*modx+ mody*mody +modz*modz)<=tolerence){
                    
                Node1=NodeMap1->first; NofElementsCreated+=1;NofNodesCreated++;                   
                Node2=It2->second; string str = "element";
                // cout << "New Elements Created" << endl; 
                string ElementDesc = to_string(this->GmshParse.getElementList().size()+1) + " 1 2 "  + to_string(newPhysicalGroupTag) + " " + to_string(EntityNo)  + " " + to_string(Node1) + " " + to_string(Node2);
                // cout << endl << ElementDesc ;
                Element newElement = Element(ElementDesc);
                this->GmshParse.addElement(newElement);
                this->ElementNoMap.insert(pair<int,int>(newElement.getId(),0));
                newNodeElement.ElementList.push_back(newElement);
                newNodeElement.NodeList.insert(pair<int,int>(Node1,Node1));
                newNodeElement.NodeList.insert(pair<int,int>(Node2,Node2));
                UniqueNodes++;

                if(UniqueNodes>1){ string str =  "\n \t \033[1;31mERROR:: More than one node inside tolerence \033[0m\n"; throw str.c_str();}
            }
        }
    }

    cout << "\n \t Sucessfully Converted" << endl;

    if (NofElementsCreated ==0){

        cout << "\n \t \033[1;33mWarning:: No elements were created \033[0m\n";
        return;
    }

    this->PhysicalGroupMap.insert(pair<int,NodeElement>(newPhysicalGroupTag,newNodeElement));
    string PhysicDes = "1 "+ to_string(newPhysicalGroupTag)+  " \"" + newPhysicalGroupName +  "\""; 
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
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
    /// Ends Initialization here

    map<string,int>::iterator MaterialTagIter;
    map<string,Semantics>::iterator TempFunctionIter;

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
  
    string CompoundVariable = Variables.at(init-1); string delimiters = "[]";
    CompoundVariable.erase( CompoundVariable.find_last_not_of( delimiters ) + 1 ).erase( 0, CompoundVariable.erase( CompoundVariable.find_last_not_of( delimiters ) + 1 ).find_first_not_of( delimiters ) );
    Tokenizer tknzr = Tokenizer(trim(CompoundVariable),"{");

    string gmssiCommandtag, gmssiArguments;

    try{
        if(tknzr.countTokens()!=2) throw exception();
        gmssiCommandtag =tknzr.nextToken()+"{";
        tknzr.setDelimiter("");
        gmssiArguments = tknzr.nextToken();
    } catch (exception& e) { string str = "\n \t \033[1;31mERROR:: Syntax Error in " +  Variables.at(init-1) + " \033[0m\n" ; throw str.c_str();}

    // replace( gmssiArguments.begin(), gmssiArguments.end(), ';', ',' );
    // tknzr.set(gmssiArguments,",");

    // if(tknzr.countTokens()>=1) gmssiArguments=","+gmssiArguments;

    if (gmssiArguments.substr(0,gmssiArguments.length()-1).compare(""))
        gmssiArguments=","+gmssiArguments;

    string ElementalCommand =  gmssiCommandtag +"PhyEntyGroup,1"+ gmssiArguments;
    // cout << ElementalCommand << endl;
    PhysicalGroup TempPhyGroup = PhysicalGroup(); TempPhyGroup.Process(ElementalCommand);
    TempFunctionIter = this->FunctionMap.find(TempPhyGroup.getCommandList().at(0));

    if (TempFunctionIter == this->FunctionMap.end()){
        string str =  "\n \t \033[1;31mError:: The gmessi command \'" + Variables.at(init-1) + "\'" + "could not be found\033[0m\n" ;
        throw str.c_str();
    }

    int nofRun=0;

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=init, MatTag=0;

        if( !(TempFunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType())))){

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

            } catch (exception& e) { string str = "\n \t \033[1;31mERROR:: Syntax Error " +  Variables.at(n-1) + " \033[0m\n" ; throw str.c_str(); }

            if(n<Variables.size()){
            
                string msg = "\n \t \033[1;31mERROR:: The command has syntaxERROR in Phy/Enty# tag\033[0m\n" ;
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

            ElementalCommand = gmssiCommandtag +"PhyEntyTag,"+to_string(MatTag)+ gmssiArguments;

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

                    /******************************** OPtimizing Elements for ESSI *****************************************/
                    if(this->ElementNoMap.find(ElementList.at(k).getId())->second==0){
                    	string NewElementNo = this->getVariable(var);
                    	this->TempVariable.push(NewElementNo);
                    	this->ElementNoMap.find(ElementList.at(k).getId())->second = stoi(NewElementNo);
                    }
                    else
                    	this->TempVariable.push(to_string(this->ElementNoMap.find(ElementList.at(k).getId())->second));
                    /******************************************************************************************************/
                }
                else if(!var.compare("node") || !var.compare("nodes")){

                    /******************************** OPtimizing Nodes for ESSI *****************************************/
                    m=m+1;if(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second==0){
                        string NewNodeNo = this->getVariable(var);
                        this->TempVariable.push(NewNodeNo);
                        this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second = stoi(NewNodeNo);
                    }
                    else
                        this->TempVariable.push(to_string(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second));
                    /*********************************************************************************************/

                    // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));
                    // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][++m]-1)));  
                }
                else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                    this->TempVariable.push(this->getVariable(var)); 
                }
                else{string UserVariable = NewVariables.at(n++);
                     if(!UserVariable.compare("PhyEntyTag"))
                        UserVariable = NewVariables.at(n++);

	                try{
	                    string unit="", prec="0", value, function;
	                    Tokenizer tknzr = Tokenizer(UserVariable,"\\");
	                    string ScriptFunction = tknzr.nextToken();
	                    function = ScriptVariables + ScriptFunction + ";";
	                    value = this->Evaluate.eval(function);
	                    if(tknzr.hasMoreTokens()){
	                        string temp = delSpaces(tknzr.nextToken()); 
	                        if (temp.compare("")) prec = temp ;
	                        value = to_string(roundToSignificantFigures(stof(this->Evaluate.eval(function)),stoi(prec)));
	                    }
	                    if(tknzr.hasMoreTokens()){ unit = unit + "*" + delSpaces(tknzr.nextToken()); }

	                    value = value + unit;          
	                    this->TempVariable.push(value);  
	                     /****************************************************** Updating Essi Tag *********************************************************/
	                   	 Tokenizer temptknzr = Tokenizer(TempFunctionIter->second.getEssiVarList().at(l),"#");
	                   	 string tempvar = temptknzr.nextToken(); 
	                   	 map<string,int>::iterator EssiTagVariableMapIter = this->EssiTagVariableMap.find(tempvar);
	                   	 if (EssiTagVariableMapIter!= this->EssiTagVariableMap.end()){ EssiTagVariableMapIter->second = stoi(UserVariable)+1;}
	                   	/****************************************************** Updating Essi Tag *********************************************************/
	                } catch (exception& e) { string str = "\n \t \033[1;31mERROR:: Syntax Error " +  Variables.at(n-1) + " \033[0m\n" ; throw str.c_str(); }
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

void gmESSITranslator::NodalVariationalCommand(const int&i, const int& j){

    ofstream LoadFile(loadFile,ios::app); int init =0;
    LoadFile<< PrintStartConversion(j);

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
    /// Ends Initialization here

    map<int,int> NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    int nofRun=0;double x_cord=0, y_cord=0, z_cord=0;

    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){

        int n=init;
        x_cord = NodeMap.find(it->second)->second.getXcord();
        y_cord = NodeMap.find(it->second)->second.getYcord();
        z_cord = NodeMap.find(it->second)->second.getZcord();
        string ScriptVariables = "x =" + to_string(x_cord) + ";" +  "y =" +   to_string(y_cord) + ";" + "z =" + to_string(z_cord) + ";";

        for(int l=0 ; l<NofEssiVariables ;l++ ){

            nofRun++;

            Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
            string var = tknzr.nextToken();
             
            if(!var.compare("node") || !var.compare("nodes")){

                /******************************** OPtimizing Nodes for ESSI *****************************************/
                if(this->NodeNoMap.find(it->second)->second==0){
                    string NewNodeNo = this->getVariable(var);
                    this->TempVariable.push(NewNodeNo);
                    this->NodeNoMap.find(it->second)->second = stoi(NewNodeNo);
                }
                else
                    this->TempVariable.push(to_string(this->NodeNoMap.find(it->second)->second));
                /*********************************************************************************************/


                // this->TempVariable.push(to_string(it->second));  
            }
            else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                this->TempVariable.push(this->getVariable(var)); 
            }
            else {string UserVariable = Variables.at(n++);

                try{

                    string unit="", prec="0", value, function;
                    Tokenizer tknzr = Tokenizer(UserVariable,"\\");
                    string ScriptFunction = tknzr.nextToken();
                    // cout << UserVariable << " " <<  ScriptFunction << endl;;
                    function = ScriptVariables + ScriptFunction + ";";
                    value = this->Evaluate.eval(function);
                    if(tknzr.hasMoreTokens()){
                        string temp = delSpaces(tknzr.nextToken()); 
                        if (temp.compare("")) prec = temp ;
                        value = to_string(roundToSignificantFigures(stof(this->Evaluate.eval(function)),stoi(prec)));
                    }
                    if(tknzr.hasMoreTokens()){ unit = unit + "*" + delSpaces(tknzr.nextToken()); }

                    value = value + unit;          
                    this->TempVariable.push(value);  
                    UpdateEssiTags(value,l);
                } catch (exception& e) { string str = "\n \t \033[1;31mERROR:: Syntax Error " +  Variables.at(n-1) + " \033[0m\n" ; throw str.c_str(); }
            }
        }

        if(n<Variables.size()){

            string msg = "\n \t \033[1;31mERROR:: The command1 has syntaxERROR in Phy/Enty# tag\033[0m\n" ; 
            this->clear(this->TempVariable);
            throw msg.c_str();
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    LoadFile << PrintEndConversion(nofRun,j);
    LoadFile.close();
    remove( "script" );
    return;

}

void gmESSITranslator::GeneralElementalVariationalCommand(const int&i, const int& j){

    ofstream LoadFile(loadFile,ios::app); int init=0;
    LoadFile<< PrintStartConversion(j);

    // cout << "I am in General Element Variational Command" << endl;

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
    /// Ends Initialization here

    vector<Element> ElementList =TypeIter->second.ElementList;
    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    int nofRun=0;

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=init, ElemNodeSize = ElementList.at(k).getNodeList().size();
        double x_cord=0, y_cord=0, z_cord=0;

        for(int z=0 ; z<ElemNodeSize ;z++ ){

            map<int,Node>::iterator NodeInfo = this->NodeMap.find(ElementList.at(k).getNodeList().at(z));
            x_cord = x_cord + NodeInfo->second.getXcord();
            y_cord = y_cord + NodeInfo->second.getYcord();
            z_cord = z_cord + NodeInfo->second.getZcord();
        }

        x_cord = x_cord/ElemNodeSize; y_cord = y_cord/ElemNodeSize; z_cord = z_cord/ElemNodeSize;
        string ScriptVariables = "x =" + to_string(x_cord) + ";" +  "y =" +   to_string(y_cord) + ";" + "z =" + to_string(z_cord) + ";";

        for(int l=0 ; l<NofEssiVariables ;l++ ){

            nofRun++;

            Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
            string var = tknzr.nextToken();
           
            if(!var.compare("element")){

                /******************************** OPtimizing Elements for ESSI *****************************************/
                if(this->ElementNoMap.find(ElementList.at(k).getId())->second==0){
                	string NewElementNo = this->getVariable(var);
                	this->TempVariable.push(NewElementNo);
                	this->ElementNoMap.find(ElementList.at(k).getId())->second = stoi(NewElementNo);
                }
                else
                	this->TempVariable.push(to_string(this->ElementNoMap.find(ElementList.at(k).getId())->second));
                /******************************************************************************************************/
            }
            else if(!var.compare("node") || !var.compare("nodes")){

                /******************************** OPtimizing for ESSI *****************************************/
                m=m+1;if(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second==0){
                    string NewNodeNo = this->getVariable(var);
                    this->TempVariable.push(NewNodeNo);
                    this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second = stoi(NewNodeNo);
                }
                else
                    this->TempVariable.push(to_string(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second));
                /*********************************************************************************************/

                // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
            }
            else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                this->TempVariable.push(this->getVariable(var)); 
            }
            else {string UserVariable = Variables.at(n++);
                try{

                    string unit="", prec="0", value, function;
                    Tokenizer tknzr = Tokenizer(UserVariable,"\\");
                    string ScriptFunction = tknzr.nextToken();
                    function = ScriptVariables + ScriptFunction + ";";
                    value = this->Evaluate.eval(function);
                    if(tknzr.hasMoreTokens()){
                        string temp = delSpaces(tknzr.nextToken()); 
                        if (temp.compare("")) prec = temp ;
                        value = to_string(roundToSignificantFigures(stof(this->Evaluate.eval(function)),stoi(prec)));
                    }
                    if(tknzr.hasMoreTokens()){ unit = unit + "*" + delSpaces(tknzr.nextToken()); }

                    value = value + unit;          
                    this->TempVariable.push(value);  
                    UpdateEssiTags(value,l);
                } catch (exception& e) { string str = "\n \t \033[1;31mERROR:: Syntax Error " +  Variables.at(n-1) + " \033[0m\n" ; throw str.c_str(); }
            }
        }

        if(n<Variables.size()){
            
            string msg = "\n \t \033[1;31mERROR:: The command has syntaxERROR in Phy/Enty# tag\033[0m\n" ;
            this->clear(this->TempVariable);
            throw msg.c_str();
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    LoadFile << PrintEndConversion(nofRun,j);
    LoadFile.close();
    remove( "script" );
    return;
}

void gmESSITranslator::ElementalVariationalCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile, ios::app); int init=0;
    GeometryFile << PrintStartConversion(j);

    // cout << "I am in Elemental Variational Command" << endl;

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
    /// Ends Initialization here

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    int nofRun=0; 

    for(int k =0;k<ElementListSize ; k++){

        int m =0,n=init , ElemNodeSize = ElementList.at(k).getNodeList().size();
        double x_cord=0, y_cord=0, z_cord=0;

        for(int z=0 ; z<ElemNodeSize ;z++ ){

            map<int,Node>::iterator NodeInfo = this->NodeMap.find(ElementList.at(k).getNodeList().at(z));
            x_cord = x_cord + NodeInfo->second.getXcord();
            y_cord = y_cord + NodeInfo->second.getYcord();
            z_cord = z_cord + NodeInfo->second.getZcord();
        }

        x_cord = x_cord/ElemNodeSize; y_cord = y_cord/ElemNodeSize; z_cord = z_cord/ElemNodeSize;
        string ScriptVariables = "x =" + to_string(x_cord) + ";" +  "y =" +   to_string(y_cord) + ";" + "z =" + to_string(z_cord) + ";";

        if( !(this->FunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

            nofRun++;

            for(int l=0 ; l<NofEssiVariables ;l++ ){

                Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                string var = tknzr.nextToken();
             
                if(!var.compare("element")){

                    /******************************** OPtimizing Elements for ESSI *****************************************/
                    if(this->ElementNoMap.find(ElementList.at(k).getId())->second==0){
                    	string NewElementNo = this->getVariable(var);
                    	this->TempVariable.push(NewElementNo);
                    	this->ElementNoMap.find(ElementList.at(k).getId())->second = stoi(NewElementNo);
                    }
                    else
                    	this->TempVariable.push(to_string(this->ElementNoMap.find(ElementList.at(k).getId())->second));
                    /******************************************************************************************************/
                }
                else if(!var.compare("node") || !var.compare("nodes")){

                    /******************************** OPtimizing for ESSI *****************************************/
                    m=m+1;if(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second==0){
                        string NewNodeNo = this->getVariable(var);
                        this->TempVariable.push(NewNodeNo);
                        this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second = stoi(NewNodeNo);
                    }
                    else
                        this->TempVariable.push(to_string(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m]-1))->second));
                    /*********************************************************************************************/

                    // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));                   
                }
                else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                    this->TempVariable.push(this->getVariable(var)); 
                }
                else {string UserVariable = Variables.at(n++); 
	                try{

	                    string unit="", prec="0", value, function;
	                    Tokenizer tknzr = Tokenizer(UserVariable,"\\");
	                    string ScriptFunction = tknzr.nextToken();
	                    function = ScriptVariables + ScriptFunction + ";";
	                    // cout << "function " << function << endl;
	                    value = this->Evaluate.eval(function);
	                    if(tknzr.hasMoreTokens()){
	                        string temp = delSpaces(tknzr.nextToken()); 
	                        if (temp.compare("")) prec = temp ;
	                        value = to_string(roundToSignificantFigures(stof(this->Evaluate.eval(function)),stoi(prec)));
	                    }
	                    if(tknzr.hasMoreTokens()){ unit = unit + "*" + delSpaces(tknzr.nextToken()); }

	                    value = value + unit;          
	                    this->TempVariable.push(value);  
	                    UpdateEssiTags(value,l);
	                } catch (exception& e) { string str = "\n \t \033[1;31mERROR:: Syntax Error " +  Variables.at(n-1) + " \033[0m\n" ; throw str.c_str(); }
                }
            }

            if(n<Variables.size()){
            
                string msg = "\n \t \033[1;31mERROR:: The command has syntaxERROR in Phy/Enty# tag\033[0m\n" ;
                this->clear(this->TempVariable); 
                throw msg.c_str();
            }

            GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
        }
    }

    GeometryFile << PrintEndConversion(nofRun,j);
    GeometryFile.close();
    remove( "script" );
    return;
}

void gmESSITranslator::ElementalCompoundVariationalCommand(const int&i, const int& j){

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

    map<int,NodeElement>::iterator TypeIter;
    map<int,NodeElement>::iterator SurfaceIter;

    if(VariablesToFind+VariablesByUser > NofEssiVariables+1){
        if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
        if( setTypeIter(SurfaceIter,this->VariableList.at(j).at(1))<0)
            return;
    }
    else{
        TypeIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
        if( setTypeIter(SurfaceIter,this->VariableList.at(j).at(0))<0)
            return;
    }

    vector<Element> ElementList =TypeIter->second.ElementList;
    map<int,int>  NodeList =TypeIter->second.NodeList;
    int ElementListSize = ElementList.size();

    int nofRun=0;

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=init+1 , ElemNodeSize = ElementList.at(k).getNodeList().size();
        double x_cord=0, y_cord=0, z_cord=0;

        for(int z=0 ; z<ElemNodeSize ;z++ ){

            map<int,Node>::iterator NodeInfo = this->NodeMap.find(ElementList.at(k).getNodeList().at(z));
            x_cord = x_cord + NodeInfo->second.getXcord();
            y_cord = y_cord + NodeInfo->second.getYcord();
            z_cord = z_cord + NodeInfo->second.getZcord();
        }

        x_cord = x_cord/ElemNodeSize; y_cord = y_cord/ElemNodeSize; z_cord = z_cord/ElemNodeSize;
        string ScriptVariables = "x =" + to_string(x_cord) + ";" +  "y =" +   to_string(y_cord) + ";" + "z =" + to_string(z_cord) + ";";

        if( !(this->FunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

            nofRun++;

            for(int l=0 ; l<NofEssiVariables ;l++ ){

                Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                string var = tknzr.nextToken();
             
                if(!var.compare("element")){

                    /******************************** OPtimizing Elements for ESSI *****************************************/
                    if(this->ElementNoMap.find(ElementList.at(k).getId())->second==0){
                    	string NewElementNo = this->getVariable(var);
                    	this->TempVariable.push(NewElementNo);
                    	this->ElementNoMap.find(ElementList.at(k).getId())->second = stoi(NewElementNo);
                    }
                    else
                    	this->TempVariable.push(to_string(this->ElementNoMap.find(ElementList.at(k).getId())->second));
                    /******************************************************************************************************/
                }
                else if(!var.compare("node") || !var.compare("nodes")){

                    bool loop=true; int loopMax =  ElementList.at(k).getNodeList().size();

                    while(loop && m<loopMax){
                         
                        if(SurfaceIter==this->PhysicalGroupMap.end()||SurfaceIter==this->EntityMap.end()){

                            string msg = "\n \t \033[1;31mERROR:: The command failed to convert as there is no such Physical/Entity Group\033[0m\n";
                            throw msg.c_str(); 
                        }

                        map<int,int>::iterator NodeIter = SurfaceIter->second.NodeList.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m+1]-1));
                        
                        // map<int,int>::iterator NodeIter = SurfaceIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));
                        map<int,int>::iterator NodeIterEnd = SurfaceIter->second.NodeList.end();

                        // if(NodeIter==SurfaceIter->second.NodeList.end()){

                        //     string msg = "\n \t \033[1;33mWARNING:: The command could not find any nodes/elements on which it operates\033[0m\n" ; 
                        //     cout <<  msg;   
                        // }

                        if(NodeIter!= NodeIterEnd){

                            /******************************** OPtimizing Nodes for ESSI *****************************************/
                            if(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m+1]-1))->second==0){
                                string NewNodeNo = this->getVariable(var);
                                this->TempVariable.push(NewNodeNo);
                                this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m+1]-1))->second = stoi(NewNodeNo);
                            }
                            else
                                this->TempVariable.push(to_string(this->NodeNoMap.find(ElementList.at(k).getNodeList().at(GMSH_to_ESSI_NODE_CONNECTIVITY[ElementList.at(k).getType()][m+1]-1))->second));
                            /*********************************************************************************************/
                                    
                            // this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
                            loop = false;
                        }

                        m++;
                    
                    }
                }
                else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){ 
                    this->TempVariable.push(this->getVariable(var)); 
                }
                else {
                    string UserVariable = Variables.at(n++);
	                try{

	                    string unit="", prec="0", value, function;
	                    Tokenizer tknzr = Tokenizer(UserVariable,"\\");
	                    string ScriptFunction = tknzr.nextToken();
	                    function = ScriptVariables + ScriptFunction + ";";
	                    value = this->Evaluate.eval(function);
	                    if(tknzr.hasMoreTokens()){
	                        string temp = delSpaces(tknzr.nextToken()); 
	                        if (temp.compare("")) prec = temp ;
	                        	value = to_string(roundToSignificantFigures(stof(this->Evaluate.eval(function)),stoi(prec)));
	                    }
	                    if(tknzr.hasMoreTokens()){ unit = unit + "*" + delSpaces(tknzr.nextToken()); }

	                    value = value + unit;          
	                    this->TempVariable.push(value);  
	                    UpdateEssiTags(value,l);
	                } catch (exception& e) { string str = "\n \t \033[1;31mERROR:: Syntax Error " +  UserVariable + " \033[0m\n" ; throw str.c_str();}
                }
            }

            if(n<Variables.size()){
                string msg = "\n \t \033[1;31mERROR:: The command1 has syntaxERROR in Phy/Enty# tag\033[0m\n" ; 
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
                    if(!var.compare("node") || !var.compare("nodes") || !var.compare("element")) continue;
                    else if (itr != this->EssiTagVariableMap.end()) itr->second = itr->second -1; 
                }

            }
        }
    }

    LoadFile << PrintEndConversion(nofRun,j);
    LoadFile.close();
    remove( "script" );
    return;
}


void gmESSITranslator::WriteCommand(const int&i, const int& j){

    // Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter; int init=0; string node = "node", element="element"; 
    if( setTypeIter(TypeIter,this->VariableList.at(j),i,j,init)<0)
        return;
    /// Ends Initialization here

    vector<string> Variables = VariableList.at(j);
    string NodesFileName = this->pwd +"_" + VariableList.at(j).at(init) +"_Nodes.txt";
    string ElementsFileName = this->pwd +"_" + VariableList.at(j).at(init) +"_Elements.txt";
    ofstream NodesFile(NodesFileName,ios::out); 
    ofstream ElementsFile(ElementsFileName,ios::out); 

    vector<Element> ElementList = TypeIter->second.ElementList;
    int ElementListSize = ElementList.size();

    for(int i=0; i<ElementListSize ; i++){
        
        /******************************** OPtimizing Elements for ESSI *****************************************/
        if(this->ElementNoMap.find(ElementList.at(i).getId())->second==0){
            string NewElementNo = this->getVariable(element);
            ElementsFile << NewElementNo << "\t";
            this->ElementNoMap.find(ElementList.at(i).getId())->second = stoi(NewElementNo);
        }
        else
           ElementsFile << this->ElementNoMap.find(ElementList.at(i).getId())->second << "\t";
        /******************************************************************************************************/
       
        // ElementsFile << ElementList.at(i).getId() << "\t";
        ElementsFile << ElementList.at(i).getType() << "\t";
        int size =  ElementList.at(i).getNodeList().size();

        for( int j =0 ;j<size ; j++ ){            
            
            /******************************** OPtimizing Nodes for ESSI *****************************************/
            if(this->NodeNoMap.find(ElementList.at(i).getNodeList().at(j))->second==0){
                string NewNodeNo = this->getVariable(node);
                ElementsFile << NewNodeNo << "\t";
                this->NodeNoMap.find(ElementList.at(i).getNodeList().at(j))->second = stoi(NewNodeNo);
            }
            else
                ElementsFile << this->NodeNoMap.find(ElementList.at(i).getNodeList().at(j))->second << "\t";
            /*********************************************************************************************/
            // ElementsFile << ElementList.at(i).getNodeList().at(j) << "\t";
        }
        ElementsFile << "\n";
    }

    map<int,int> NodeList =TypeIter->second.NodeList;
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();

    int nofRun=0;
    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){
        
        nofRun++;
        map<int,Node>::iterator NodeInfo = this->NodeMap.find(it->second);

        /******************************** OPtimizing Nodes for ESSI *****************************************/
        if(this->NodeNoMap.find(NodeInfo->second.getId())->second==0){
            string NewNodeNo = this->getVariable(node);
            NodesFile << NewNodeNo << "\t";
            this->NodeNoMap.find(NodeInfo->second.getId())->second = stoi(NewNodeNo);
        }
        else
            NodesFile << to_string(this->NodeNoMap.find(NodeInfo->second.getId())->second) << "\t";
        /*********************************************************************************************/
        // NodesFile << to_string(NodeInfo->second.getId()) << "\t";
        NodesFile << to_string(NodeInfo->second.getXcord())<< "\t"; 
        NodesFile << to_string(NodeInfo->second.getYcord())<< "\t"; 
        NodesFile << to_string(NodeInfo->second.getZcord())<< "\n"; 
    }

    if(nofRun==0){

        string msg = "\n \t \033[1;33mWARNING:: The command could not find any nodes/elements on which it operates\033[0m\n" ; 
        cout << msg;
        return;        
    }

    cout << "\n \t Sucessfully Converted" << endl;
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
    Tokenizer inpString = Tokenizer(Command,"$") ;

    if(this->TempVariable.size() > NofEssiVariables) {
        string msg = "\n \t \033[1;31mERROR:: The commandhas more than required variables\033[0m\n" ; 
        throw msg.c_str();
    }

    if(this->TempVariable.size() < NofEssiVariables) {        
        string msg = "\n \t \033[1;31mERROR:: The commandhas less than required variables\033[0m\n" ; 
        throw msg.c_str();
    }
    
    while(inpString.hasMoreTokens() && nofTokens++<NofEssiVariables){
        Ecommand << inpString.nextToken()  << setw(0);
        Ecommand << TempVariable.front()   << setw(0);
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

int gmESSITranslator::setTypeIter(map<int,NodeElement>::iterator &TypeIter,const vector<string>& variable,const int& i,const int& j, int &n){

    if(variable.size()>0){

        Tokenizer tknzr = Tokenizer((variable.at(0)),"#");
        int NofTokens = tknzr.countTokens(), tag ;

        if(NofTokens==2){

            string type = (tknzr.nextToken());

            if(!type.compare("Entity_Group")){

                try{
                    tag = stoi(tknzr.nextToken());
                } catch(exception e) {
                    string msg = "\n \t \033[1;31mERROR:: The command  failed to convert as there is no such  Entity Group  \"" + variable.at(0)  + "\" \033[0m\n" ; 
                    msg = msg +  "\t \033[1;33mWARNING:: Escaping the execution of this command \033[0m\n" ;  
                    cout << msg ;
                    return -1 ;         
                }
                
                TypeIter = this->EntityMap.find(tag);

                if(TypeIter==this->EntityMap.end()|| TypeIter->second.NodeList.size()==0){
                    string msg = "\n \t \033[1;33mWARNING:: The command failed to convert as there is no elements/nodes in the Entity Group \"" + variable.at(0) + "\" \033[0m\n" ; 
                    cout << msg;  
                    return -1 ;  
                }
            }

            else if (!type.compare("Physical_Group")){

                map<string,int>::iterator PhysicalStringNameToIdMapIter;
                PhysicalStringNameToIdMapIter = this->PhysicalStringNameToIdMap.find(tknzr.nextToken());

                if(PhysicalStringNameToIdMapIter==this->PhysicalStringNameToIdMap.end()){
                    string msg = "\n \t \033[1;31mERROR:: The command failed to convert as there is no such Physical Group \"" + variable.at(0) + "\" \033[0m\n" ; 
                    msg = msg +  "\t \033[1;33mWARNING:: Escaping the execution of this command \033[0m\n" ;  
                    cout << msg ;
                    return -1 ;   
                }

                tag = PhysicalStringNameToIdMapIter->second;
                TypeIter = this->PhysicalGroupMap.find(tag);

                if(TypeIter->second.NodeList.size()==0){
                    string msg = "\n \t \033[1;33mWARNING:: The command failed to convert as there is no elements/nodes in the Physical Group \"" + variable.at(0) + "\" \033[0m\n" ; 
                    cout << msg;   
                    return -1;  
                }
            }

            else{

                string msg = "\n \t \033[1;31mERROR:: The command has a syntaxERROR in Entity_Group/Physical_Group#Tag tag \"" + variable.at(0) + "\" \033[0m\n" ;
                msg = msg +  "\t \033[1;33mWARNING:: Escaping the execution of this command \033[0m\n" ;  
                cout << msg ;
                return -1 ;   
            }

            n=n+1;
        }

        else if (NofTokens==1){

            TypeIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

            if(TypeIter==this->PhysicalGroupMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\n \t \033[1;33mWARNING:: The command failed to convert as there is no elements/nodes in the Physical Group \033[0m\n" ; 
                cout << msg; 
                return -1;    
            }

        }
        else{

            string msg = "\n \t \033[1;31mERROR:: The command has a syntaxERROR in Entity_Group/Physical_Group#Tag tag \033[0m\n" ;
            msg = msg +  "\t \033[1;33mWARNING:: Escaping the execution of this command \033[0m\n" ;  
            cout << msg ;
            return -1 ;   
        }

    }

    else{

        TypeIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

        if(TypeIter==this->PhysicalGroupMap.end()|| TypeIter->second.NodeList.size()==0){
            string msg = "\n \t \033[1;33mWARNING:: The commanded to convert as there is no elements/nodes in the Physical Group  \033[0m\n" ; 
            return -1;      
        }
    }

    return 0 ;
}

int gmESSITranslator::setTypeIter(map<int,NodeElement>::iterator &TypeIter,const string& variable){

    Tokenizer tknzr = Tokenizer((variable),"#");
    int NofTokens = tknzr.countTokens(),tag;

    if(NofTokens==2){

        string type = (tknzr.nextToken());

        if(!type.compare("Entity_Group")){

            try{
                tag = stoi(tknzr.nextToken());
            } catch(exception e) {
                string msg = "\n \t \033[1;31mERROR:: The command  failed to convert as there is no such  Entity Group  \"" + variable  + "\" \033[0m\n" ; 
                msg = msg +  "\t \033[1;33mWARNING:: Escaping the execution of this command \033[0m\n" ;  
                cout << msg ;
                return -1 ;       
            }
                
            TypeIter = this->EntityMap.find(tag);

            if(TypeIter==this->EntityMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\n \t \033[1;33mWARNING:: The command  failed to convert as there is no elements/nodes in the Entity Group  \"" + variable + "\" \033[0m\n" ; 
                cout << msg; 
                return -1 ;    
            }
        }

        else if (!type.compare("Physical_Group")){

            map<string,int>::iterator PhysicalStringNameToIdMapIter;
            PhysicalStringNameToIdMapIter = this->PhysicalStringNameToIdMap.find(tknzr.nextToken());

            if(PhysicalStringNameToIdMapIter==this->PhysicalStringNameToIdMap.end()){
                string msg = "\n \t \033[1;31mERROR:: The command  failed to convert as there is no such Physical Group \"" + variable + "\" \033[0m\n" ; 
                msg = msg +  "\t \033[1;33mWARNING:: Escaping the execution of this command \033[0m\n" ;  
                cout << msg ;
                return -1 ;    
            }

            tag = PhysicalStringNameToIdMapIter->second;
            TypeIter = this->PhysicalGroupMap.find(tag);

            if(TypeIter->second.NodeList.size()==0){
                string msg = "\n \t \033[1;33mWARNING:: The command  failed to convert as there is no elements/nodes in the Physical Group \"" + variable + "\" \033[0m\n" ; 
                cout << msg;     
            }
        }

        else{

            string msg = "\n \t \033[1;31mERROR:: The command  has a syntaxERROR in " +  variable  + " \033[0m\n" ;
            msg = msg +  "\t \033[1;33mWARNING:: Escaping the execution of this command \033[0m\n" ;  
            cout << msg ;
            return -1 ; 
        }
    }

    else{

        string msg = "\n \t \033[1;31mERROR:: The command  has a syntaxERROR in " +  variable  + " \033[0m\n" ;
        msg = msg +  "\t \033[1;33mWARNING:: Escaping the execution of this command \033[0m\n" ;  
        cout << msg ;
        return -1 ; 
    }

    return 0;
}

string gmESSITranslator::PrintStartConversion(const int& j){

    string str = "\n//*************************************************************************************************************************\n";
    str = str + "//\t\t\t\t\t\t\t" +  this->UserCommandList.at(j) + "Starts\n";
    str = str + "//*************************************************************************************************************************\n\n" ;
    return str;
}

string gmESSITranslator::PrintEndConversion(const int& nofRun, const int& j){

    if(nofRun==0){

        string msg = "\n \t \033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'could not find any nodes/elements on which it operates\033[0m\n" ; 
        cout << msg;     
    }

    cout << "\n \t Sucessfully Converted" << endl;

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

    } catch(exception& e) { string str = "\n \t \033[1;31mERROR:: Syntax Error in The command. The parameter "+ tempvar +"_no should be a integer \033[0m\n" ; throw str.c_str();}

    return;
}

void gmESSITranslator::Convert(const string& GmssiCommand){

    this->GMSH_to_ESSI_NODE_Mapping();
    int i = PhytonScriptPhysicalGroupIndex;
    PhysicalGroupList.at(i).Process(GmssiCommand);

    this->CommandList = this->PhysicalGroupList.at(i).getCommandList();
    this->VariableList = this->PhysicalGroupList.at(i).getVariableList();
    this->NofVariablesList = this->PhysicalGroupList.at(i).getNofVariables();
    this->UserCommandList = this->PhysicalGroupList.at(i).getUserCommandList();
    int j = CommandList.size()-1;     
    this->FunctionIter = this->FunctionMap.find(this->CommandList.at(j));

    cout << "\n" << this->UserCommandList.at(j) ;

    if (this->FunctionIter != this->FunctionMap.end()){
        
        cout<< "\n \t "<< "Found!!";

        if(this->FunctionIter->second.getMatchMode() && !(this->FunctionIter->second.getSemanticsId().compare("c")))
            this->ElementalCompoundCommand(i,j);
        else if(this->FunctionIter->second.getMatchMode() && !(this->FunctionIter->second.getSemanticsId().compare("vc")))
            this->ElementalCompoundVariationalCommand(i,j);
        else if(this->FunctionIter->second.getMatchMode() && !(this->FunctionIter->second.getSemanticsId().compare("v")))
            this->ElementalVariationalCommand(i,j);
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
        else if (!this->FunctionIter->second.getElementId().compare("nv"))
            this->NodalVariationalCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("ev"))
            this->GeneralElementalVariationalCommand(i,j);
        else if (!this->FunctionIter->second.getElementId().compare("wr"))
            this->WriteCommand(i,j);
    }
    else{
        
        cout << "\n \t \033[1;31mNot Found!!" << "\033[0m";
        cout << "\n \t \033[1;33mWARNING:: Execuation of the command escaped. The gmessi command could not be found" << " \033[0m\n" ; 
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

void gmESSITranslator::Generate_Make_File(){

    string MakeFile_File = this->ESSI_Simulation_Dir +"Makefile";
    ofstream Makefile(MakeFile_File,ios::out);  
    Makefile << endl << "# Default Makefile Variables " << endl;
    Makefile << "NP=4 " << endl << endl;
    Makefile << "all: " << endl;
    Makefile << "\t essi -f " << mainFile << endl << endl;
    Makefile << "serial: " << endl;
    Makefile << "\t essi -f " << mainFile << endl << endl;   
    Makefile << "parallel: " << endl;
    Makefile << "\t mpirun -np ${NP} pessi -f " << mainFile << endl << endl; 
    Makefile << "clean: " << endl;
    Makefile << "\t rm *.feioutput "<< endl << endl; 

    return;
}

int gmESSITranslator::addNode(Node node){

    int nodeId = node.getId();

    if(NodeNoMap.find(nodeId)!=NodeNoMap.end())
        return -1; // Node no already exists

    this->GmshParse.addNode(node);
    NodeMap.insert(pair<int,Node>(nodeId,node));
    this->NodeNoMap[nodeId]=0;

    return 0;


}

int gmESSITranslator::addElement(Element ele){

    int eleId = ele.getId();

    if(ElementNoMap.find(eleId)!=ElementNoMap.end())
        return -1; // Element no already exists

    this->GmshParse.addElement(ele);
    this->ElementNoMap[eleId]=0;

    return 0;
}


void gmESSITranslator::GMSH_to_ESSI_NODE_Mapping(){

    // int GMSH_to_ESSI_NODE_CONNECTIVITY[18][28];

    // for (int i=0;i<18;i++){
    //     for (int j=0;j<28;j++){
    //         GMSH_to_ESSI_NODE_CONNECTIVITY[i][j]=0;
    //         // cout << i << " " << j << endl;
    //     }
    // }

    // Elemental Commands  --- 1 2-node line.

        GMSH_to_ESSI_NODE_CONNECTIVITY [1][1]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [1][2]=2;
   
    //Elemental Commands  --- 2 3-node triangle.
        
        GMSH_to_ESSI_NODE_CONNECTIVITY [2][1]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [2][2]=2;  GMSH_to_ESSI_NODE_CONNECTIVITY [2][3]=3;  

    //Elemental Commands  --- 3 4-node quadrangle.

        GMSH_to_ESSI_NODE_CONNECTIVITY [3][1]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [3][2]=2;  GMSH_to_ESSI_NODE_CONNECTIVITY [3][3]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [3][4]=4;     

    //Elemental Commands  --- 4 4-node tetrahedron.
    //Elemental Commands  --- 5 8-node hexahedron.
        
        GMSH_to_ESSI_NODE_CONNECTIVITY [5][1]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][2]=8;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][3]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][4]=6; 
        GMSH_to_ESSI_NODE_CONNECTIVITY [5][5]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][6]=4;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][7]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][8]=2;

        // GMSH_to_ESSI_NODE_CONNECTIVITY [5][1]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][2]=2;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][3]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][4]=4; 
        // GMSH_to_ESSI_NODE_CONNECTIVITY [5][5]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][6]=6;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][7]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][8]=8;

        // GMSH_to_ESSI_NODE_CONNECTIVITY [5][1]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][2]=2;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][3]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][4]=4; 
        // GMSH_to_ESSI_NODE_CONNECTIVITY [5][5]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][6]=6;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][7]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [5][8]=8;

    //Elemental Commands  --- 6 6-node prism.
    //Elemental Commands  --- 7 5-node pyramid.
    //Elemental Commands  --- 8 3-node second order line (2 nodes associated with the vertices and 1 with the edge).
    //Elemental Commands  --- 9 6-node second order triangle (3 nodes associated with the vertices and 3 with the edges).
    //Elemental Commands  --- 10 9-node second order quadrangle (4 nodes associated with the vertices, 4 with the edges and 1 with the face).

        GMSH_to_ESSI_NODE_CONNECTIVITY [10][1]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [10][2]=2;  GMSH_to_ESSI_NODE_CONNECTIVITY [10][3]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [10][4]=4; 
        GMSH_to_ESSI_NODE_CONNECTIVITY [10][5]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [10][6]=6;  GMSH_to_ESSI_NODE_CONNECTIVITY [10][7]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [10][8]=8;
        GMSH_to_ESSI_NODE_CONNECTIVITY [10][9]=9;

    //Elemental Commands  --- 11 10-node second order tetrahedron (4 nodes associated with the vertices and 6 with the edges).u
    //Elemental Commands  --- 12 27-node second order hexahedron (8 nodes associated with the vertices, 12 with the edges, 6 with the faces and 1 with the volume).

        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][1]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][2]=8;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][3]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][4]=6; 
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][5]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][6]=4;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][7]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][8]=2;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][9]=15;   GMSH_to_ESSI_NODE_CONNECTIVITY [12][10]=14;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][11]=19;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][12]=16;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][13]=20;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][14]=13;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][15]=17;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][16]=18;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][17]=11;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][18]=10;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][19]=12;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][20]=9;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][21]=27;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][22]=24;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][23]=23;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][24]=25;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][25]=22;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][26]=26;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][27]=21;

        GMSH_to_ESSI_NODE_CONNECTIVITY [12][1]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][2]=8;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][3]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][4]=6; 
        GMSH_to_ESSI_NODE_CONNECTIVITY [12][5]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][6]=4;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][7]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][8]=2;
        GMSH_to_ESSI_NODE_CONNECTIVITY [12][9]=20;   GMSH_to_ESSI_NODE_CONNECTIVITY [12][10]=18;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][11]=17;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][12]=19;
        GMSH_to_ESSI_NODE_CONNECTIVITY [12][13]=14;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][14]=10;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][15]=9;   GMSH_to_ESSI_NODE_CONNECTIVITY [12][16]=12;
        GMSH_to_ESSI_NODE_CONNECTIVITY [12][17]=15;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][18]=16;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][19]=11;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][20]=13;        
        GMSH_to_ESSI_NODE_CONNECTIVITY [12][21]=27;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][22]=25;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][23]=23;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][24]=22;
        GMSH_to_ESSI_NODE_CONNECTIVITY [12][25]=24;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][26]=26;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][27]=21;

        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][1]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][2]=2;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][3]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][4]=4; 
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][5]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][6]=6;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][7]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][8]=8;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][9]=9;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][10]=10;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][11]=11;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][12]=12;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][13]=13;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][14]=14;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][15]=15;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][16]=16;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][17]=17;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][18]=18;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][19]=19;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][20]=20;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][21]=21;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][22]=22;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][23]=23;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][24]=24;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][25]=25;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][26]=26;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][27]=27;

        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][1]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][2]=2;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][3]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][4]=4; 
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][5]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][6]=6;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][7]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [12][8]=8;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][9]=12;   GMSH_to_ESSI_NODE_CONNECTIVITY [12][10]=9;   GMSH_to_ESSI_NODE_CONNECTIVITY [12][11]=10;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][12]=14;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][13]=15;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][14]=17;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][15]=18;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][16]=20;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][17]=15;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][18]=13;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][19]=11;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][20]=16;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][21]=27;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][22]=24;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][23]=22;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][24]=23;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [12][25]=25;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][26]=21;  GMSH_to_ESSI_NODE_CONNECTIVITY [12][27]=26;
        
    //Elemental Commands  --- 13 18-node second order prism (6 nodes associated with the vertices, 9 with the edges and 3 with the quadrangular faces).
    //Elemental Commands  --- 14 14-node second order pyramid (5 nodes associated with the vertices, 8 with the edges and 1 with the quadrangular face).
    //Elemental Commands  --- 15 1-node point.
    //Elemental Commands  --- 16 8-node second order quadrangle (4 nodes associated with the vertices and 4 with the edges).
    //Elemental Commands  --- 17 20-node second order hexahedron (8 nodes associated with the vertices and 12 with the edges).

        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][1]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][2]=8;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][3]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][4]=6; 
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][5]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][6]=4;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][7]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][8]=2;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][9]=15;   GMSH_to_ESSI_NODE_CONNECTIVITY [17][10]=14;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][11]=19;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][12]=16;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][13]=20;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][14]=13;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][15]=17;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][16]=18;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][17]=11;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][18]=10;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][19]=12;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][20]=9;

        GMSH_to_ESSI_NODE_CONNECTIVITY [17][1]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][2]=8;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][3]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][4]=6; 
        GMSH_to_ESSI_NODE_CONNECTIVITY [17][5]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][6]=4;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][7]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][8]=2;
        GMSH_to_ESSI_NODE_CONNECTIVITY [17][9]=20;   GMSH_to_ESSI_NODE_CONNECTIVITY [17][10]=18;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][11]=17;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][12]=19;
        GMSH_to_ESSI_NODE_CONNECTIVITY [17][13]=14;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][14]=10;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][15]=9;   GMSH_to_ESSI_NODE_CONNECTIVITY [17][16]=12;
        GMSH_to_ESSI_NODE_CONNECTIVITY [17][17]=15;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][18]=16;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][19]=11;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][20]=13;

        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][1]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][2]=2;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][3]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][4]=4; 
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][5]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][6]=6;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][7]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][8]=8;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][9]=9;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][10]=10;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][11]=11;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][12]=12;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][13]=13;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][14]=14;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][15]=15;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][16]=16;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][17]=17;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][18]=18;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][19]=19;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][20]=20;

        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][1]=3;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][2]=2;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][3]=1;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][4]=4; 
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][5]=7;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][6]=6;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][7]=5;    GMSH_to_ESSI_NODE_CONNECTIVITY [17][8]=8;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][9]=12;   GMSH_to_ESSI_NODE_CONNECTIVITY [17][10]=9;   GMSH_to_ESSI_NODE_CONNECTIVITY [17][11]=10;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][12]=14;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][13]=15;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][14]=17;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][15]=18;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][16]=20;
        // GMSH_to_ESSI_NODE_CONNECTIVITY [17][17]=15;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][18]=13;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][19]=11;  GMSH_to_ESSI_NODE_CONNECTIVITY [17][20]=16;

    //Elemental Commands  --- 18 15-node second order prism (6 nodes associated with the vertices and 9 with the edges).
    //Elemental Commands  --- 19 13-node second order pyramid (5 nodes associated with the vertices and 8 with the edges).
    //Elemental Commands  --- 20 9-node third order incomplete triangle (3 nodes associated with the vertices, 6 with the edges)
    //Elemental Commands  --- 21 10-node third order triangle (3 nodes associated with the vertices, 6 with the edges, 1 with the face)
    //Elemental Commands  --- 22 12-node fourth order incomplete triangle (3 nodes associated with the vertices, 9 with the edges)
    //Elemental Commands  --- 23 15-node fourth order triangle (3 nodes associated with the vertices, 9 with the edges, 3 with the face)
    //Elemental Commands  --- 24 15-node fifth order incomplete triangle (3 nodes associated with the vertices, 12 with the edges)
    //Elemental Commands  --- 25 21-node fifth order complete triangle (3 nodes associated with the vertices, 12 with the edges, 6 with the face)
    //Elemental Commands  --- 26 4-node third order edge (2 nodes associated with the vertices, 2 internal to the edge)
    //Elemental Commands  --- 27 5-node fourth order edge (2 nodes associated with the vertices, 3 internal to the edge)
    //Elemental Commands  --- 28 6-node fifth order edge (2 nodes associated with the vertices, 4 internal to the edge)
    //Elemental Commands  --- 29 20-node third order tetrahedron (4 nodes associated with the vertices, 12 with the edges, 4 with the faces)
    //Elemental Commands  --- 30 35-node fourth order tetrahedron (4 nodes associated with the vertices, 18 with the edges, 12 with the faces, 1 in the volume)
    //Elemental Commands  --- 31 56-node fifth order tetrahedron (4 nodes associated with the vertices, 24 with the edges, 24 with the faces, 4 in the volume)
    //Elemental Commands  --- 92 64-node third order hexahedron (8 nodes associated with the vertices, 24 with the edges, 24 with the faces, 8 in the volume)
    //Elemental Commands  --- 93 125-node fourth order hexahedron (8 nodes associated with the vertices, 36 with the edges, 54 with the faces, 27 in the volume)
}

// ESSI Node Numbering 

// 1   ->  1, 1, 1
// 2   -> -1, 1, 1
// 3   -> -1,-1, 1
// 4   ->  1,-1, 1 
// 5   ->  1, 1,-1
// 6   -> -1, 1,-1
// 7   -> -1,-1,-1
// 8   ->  1,-1,-1
// 9   ->  0, 1, 1
// 10  -> -1, 0, 1
// 11  ->  0,-1, 1
// 12  ->  1, 0, 1
// 13  ->  0, 1,-1
// 14  -> -1, 0,-1
// 15  ->  0,-1,-1
// 16  ->  1, 0,-1
// 17  ->  1, 1, 0
// 18  -> -1, 1, 0
// 19  -> -1,-1, 0
// 20  ->  1,-1, 0
// 21  ->  0, 0, 0
// 22  ->  0, 1, 0
// 23  -> -1, 0, 0
// 24  ->  0,-1, 0
// 25  ->  1, 0, 0
// 26  ->  0, 0, 1
// 27  ->  0, 0,-1

// Gmsh Node Numbering 

// 1   -> -1,-1,-1 -> 7
// 2   ->  1,-1,-1 -> 8
// 3   ->  1, 1,-1 -> 5
// 4   -> -1, 1,-1 -> 6
// 5   -> -1,-1, 1 -> 3
// 6   ->  1,-1, 1 -> 4
// 7   ->  1, 1, 1 -> 1
// 8   -> -1, 1, 1 -> 2
// 9   ->  0,-1,-1 -> 15
// 10  -> -1, 0,-1 -> 14
// 11  -> -1,-1, 0 -> 19
// 12  ->  1, 0,-1 -> 16
// 13  ->  1,-1, 0 -> 20
// 14  ->  0, 1,-1 -> 13
// 15  ->  1, 1, 0 -> 17
// 16  -> -1, 1, 0 -> 18
// 17  ->  0,-1, 1 -> 11
// 18  -> -1, 0, 1 -> 10
// 19  ->  1, 0, 1 -> 12
// 20  ->  0, 1, 1 -> 9
// 21  ->  0, 0,-1 -> 27
// 22  ->  0,-1, 0 -> 24
// 23  -> -1, 0, 0 -> 23
// 24  ->  1, 0, 0 -> 25
// 25  ->  0, 1, 0 -> 22
// 26  ->  0, 0, 1 -> 26
// 27  ->  0, 0, 0 -> 21