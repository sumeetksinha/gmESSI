/*****************************************************************************
*                               GmshTranslator.cpp
*                               =================
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/


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

    for (unsigned i=0; i<PhysicalGroupListSize; i++){

        this->CommandList = this->PhysicalGroupList.at(i).getCommandList();
        this->VariableList = this->PhysicalGroupList.at(i).getVariableList();
        this->NofVariablesList = this->PhysicalGroupList.at(i).getNofVariables();
        this->UserCommandList = this->PhysicalGroupList.at(i).getUserCommandList();
        int CommandListSize = CommandList.size();

        for (unsigned j=0; j<CommandListSize; j++){
            
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
                    this->ContactCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("mv"))
                    this->MaterialVariationalCommand(i,j);
                else if (!this->FunctionIter->second.getElementId().compare("wr"))
                    this->DRMCommand(i,j);
            }
            else{
                
                cout << left << setw(15) << "\033[1;31mNot Found!!" << "\033[0m";
                cout << "\t" << "\033[1;33mWARNING:: Execuation of the command escaped. The essi command \'" << this->UserCommandList.at(j) << "\'" << "could not be found" << " \033[0m\n" ; 
            }
        }
    }

    map<string,int>::iterator EssiTagIterBegin = EssiTagVariableMap.begin();
    map<string,int>::iterator EssiTagIterEnd = EssiTagVariableMap.end();

    cout <<endl << endl<< "\033[1;36m************************ Updated New Tag Numberring *********************" << "\033[0m\n";
    for(map<string,int>::iterator it = EssiTagIterBegin ; it!= EssiTagIterEnd ; ++it){

        cout << "\033[1;36m" << setw(15) << it->first << " = " << it->second << "\033[0m\n";
    }
}

void GmshTranslator::AddNodeCommand(const int&i, const int& j){

    // Add Node Command

    int nofRun=0;
    ofstream GeometryFile(geometryFile,ios::app); 

    GeometryFile << "\n//*************************************************************************************************************************\n";
    GeometryFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Begins\n";  
    GeometryFile << "//*************************************************************************************************************************\n\n";

    if(!(this->UserCommandList.at(j).substr(3,3).compare("All"))){

        map<int,Node> ::iterator AllNodeBegin = this->NodeMap.begin();
        map<int,Node> ::iterator AllNodeEnd = this->NodeMap.end();

        for(map<int,Node>::iterator it=AllNodeBegin; it!=AllNodeEnd; ++it){
        
            this->TempVariable.push(to_string(it->second.getId()));
            this->TempVariable.push(to_string(it->second.getXcord())+"*"+this->VariableList.at(j).at(0)); 
            this->TempVariable.push(to_string(it->second.getYcord())+"*"+this->VariableList.at(j).at(0));
            this->TempVariable.push(to_string(it->second.getZcord())+"*"+this->VariableList.at(j).at(0)); 
            this->TempVariable.push(this->VariableList.at(j).at(1)); 

            GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
        
        }

        cout << "Sucessfully Converted" << endl;

        GeometryFile << "\n//*************************************************************************************************************************\n";
        GeometryFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Ends\n";  
        GeometryFile << "//*************************************************************************************************************************\n\n";

        GeometryFile.close();
        return;
    }

    map<int,NodeElement>::iterator PhysicalGroupMapIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

    if(PhysicalGroupMapIter==this->PhysicalGroupMap.end()|| PhysicalGroupMapIter->second.NodeList.size()==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    map<int,int> NodeList =PhysicalGroupMapIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    int NofVariables = NofVariablesList.at(j);
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();

    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){
        
        nofRun++;
        map<int,Node>::iterator NodeInfo = this->NodeMap.find(it->second);

        this->TempVariable.push(to_string(it->second));
        this->TempVariable.push(to_string(NodeInfo->second.getXcord())+"*"+this->VariableList.at(j).at(0)); 
        this->TempVariable.push(to_string(NodeInfo->second.getYcord())+"*"+this->VariableList.at(j).at(0));
        this->TempVariable.push(to_string(NodeInfo->second.getZcord())+"*"+this->VariableList.at(j).at(0)); 
        this->TempVariable.push(Variables.at(1)); 

        GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    if(nofRun==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    cout << "Sucessfully Converted" << endl;

    GeometryFile << "\n//*************************************************************************************************************************\n";
    GeometryFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Ends\n";  
    GeometryFile << "//*************************************************************************************************************************\n\n";

    GeometryFile.close();
}

void GmshTranslator::ElementalCommand(const int& i, const int& j){

    map<int,NodeElement>::iterator PhysicalGroupMapIter =this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
    
    ofstream GeometryFile(geometryFile, ios::app);

    GeometryFile << "\n//*************************************************************************************************************************\n";
    GeometryFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Begins\n";  
    GeometryFile << "//*************************************************************************************************************************\n\n";

    if(PhysicalGroupMapIter==this->PhysicalGroupMap.end() || PhysicalGroupMapIter->second.ElementList.size()==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " is not executed as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
    map<int,int>  NodeList =PhysicalGroupMapIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = this->NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
     

    // cout<< "Elemental Command";
    int nofRun=0;

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=0 ;

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
                else {
                    string newVar = Variables.at(n++);
                    this->TempVariable.push(newVar);

                    Tokenizer temptknzr = Tokenizer(this->FunctionIter->second.getEssiVarList().at(l),"#");
                    string tempvar = temptknzr.nextToken(); 
                    map<string,int>::iterator EssiTagVariablemapIter = this->EssiTagVariableMap.find(tempvar);
                    if (EssiTagVariablemapIter!= this->EssiTagVariableMap.end())   EssiTagVariablemapIter->second = stoi(newVar)+1;   
                }
            }

            GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
        }
    }

    if(nofRun==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    cout << "Sucessfully Converted" << endl;

    GeometryFile << "\n//*************************************************************************************************************************\n";
    GeometryFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Ends\n";  
    GeometryFile << "//*************************************************************************************************************************\n\n";

    GeometryFile.close();
}

void GmshTranslator::ElementalCompoundCommand(const int& i, const int& j){

    map<int,NodeElement>::iterator PhysicalGroupMapIter =this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
    
    ofstream LoadFile(loadFile,ios::app); 

    LoadFile << "\n//*************************************************************************************************************************\n";
    LoadFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Begins\n";  
    LoadFile << "//*************************************************************************************************************************\n\n";

    if(PhysicalGroupMapIter==this->PhysicalGroupMap.end()|| PhysicalGroupMapIter->second.ElementList.size()==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " is not executed as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
    map<int,int>  NodeList =PhysicalGroupMapIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = this->NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();


    // cout<< "Elemental Compound Command";
    int nofRun=0;

    Tokenizer tkr = Tokenizer(Variables.at(0),"#");
    string type = this->delSpaces(tkr.nextToken());

    if(!tkr.hasMoreTokens()){
        
        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + " has a syntaxERROR in Enty/Phy# tag" + " \033[0m\n" ;
        throw msg.c_str();
    }

    int tag = stoi(this->delSpaces(tkr.nextToken()));
        

    map<int,NodeElement>::iterator EntityMapIter = this->EntityMap.find(tag);
    map<int,NodeElement>::iterator PhysicalMapIter = this->PhysicalGroupMap.find(tag);

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=1;

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
                    this->TempVariable.push(Variables.at(n++)); 
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

    if(nofRun==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    cout << "Sucessfully Converted" << endl;

    LoadFile << "\n//*************************************************************************************************************************\n";
    LoadFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Ends\n";  
    LoadFile << "//*************************************************************************************************************************\n\n";

    LoadFile.close();
}

void GmshTranslator::NodalCommand(const int& i, const int& j){

    // cout<< "Nodal Commands ";
    int nofRun=0;

    map<int,NodeElement>::iterator PhysicalGroupMapIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

    ofstream LoadFile(loadFile,ios::app);

    LoadFile << "\n//*************************************************************************************************************************\n";
    LoadFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Begins\n";  
    LoadFile << "//*************************************************************************************************************************\n\n";

    if(PhysicalGroupMapIter==this->PhysicalGroupMap.end()|| PhysicalGroupMapIter->second.NodeList.size()==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    map<int,int> NodeList =PhysicalGroupMapIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = NofVariablesList.at(j);
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){

        int n=0 ;

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
            else {
                string newVar = Variables.at(n++);
                this->TempVariable.push(newVar);

                Tokenizer temptknzr = Tokenizer(this->FunctionIter->second.getEssiVarList().at(l),"#");
                string tempvar = temptknzr.nextToken(); 
                map<string,int>::iterator EssiTagVariablemapIter = this->EssiTagVariableMap.find(tempvar);
                if (EssiTagVariablemapIter!= this->EssiTagVariableMap.end()) EssiTagVariablemapIter->second = stoi(newVar)+1; 
            }
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    if(nofRun==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    cout << "Sucessfully Converted" << endl;

    LoadFile << "\n//*************************************************************************************************************************\n";
    LoadFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Ends\n";  
    LoadFile << "//*************************************************************************************************************************\n\n";

    LoadFile.close();
}

void GmshTranslator::GeneralElementalCommand(const int& i, const int& j){

    // cout<< "General Elemental Commands";
    int nofRun=0;

    map<int,NodeElement>::iterator PhysicalGroupMapIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

    ofstream LoadFile(loadFile,ios::app);

    LoadFile << "\n//*************************************************************************************************************************\n";
    LoadFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Begins\n";  
    LoadFile << "//*************************************************************************************************************************\n\n";

    if(PhysicalGroupMapIter==this->PhysicalGroupMap.end()|| PhysicalGroupMapIter->second.ElementList.size()==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
        cout << msg;
        return;       
    }

    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=0 ;

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
            else {
                string newVar = Variables.at(n++);
                this->TempVariable.push(newVar);

                Tokenizer temptknzr = Tokenizer(this->FunctionIter->second.getEssiVarList().at(l),"#");
                string tempvar = temptknzr.nextToken(); 
                map<string,int>::iterator EssiTagVariablemapIter = this->EssiTagVariableMap.find(tempvar);
                if (EssiTagVariablemapIter!= this->EssiTagVariableMap.end())  EssiTagVariablemapIter->second = stoi(newVar)+1; 
            }
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    if(nofRun==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    cout << "Sucessfully Converted" << endl;

    LoadFile << "\n//*************************************************************************************************************************\n";
    LoadFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Ends\n";  
    LoadFile << "//*************************************************************************************************************************\n\n";

    LoadFile.close();
}

void GmshTranslator::SingularCommand(const int& i, const int& j){

    // cout<< "Singular Commands";

    ofstream MainFile(mainFile,ios::app);

    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = NofVariablesList.at(j);
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
    int n=0;  

    for(int l=0 ; l<NofEssiVariables ;l++ ){

        Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
        string var = tknzr.nextToken();
    
        if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
           this->TempVariable.push(this->getVariable(var));
        }
        else {
            string newVar = Variables.at(n++);
            this->TempVariable.push(newVar);

            Tokenizer temptknzr = Tokenizer(this->FunctionIter->second.getEssiVarList().at(l),"#");
            string tempvar = temptknzr.nextToken(); 
            map<string,int>::iterator EssiTagVariablemapIter = this->EssiTagVariableMap.find(tempvar);
            if (EssiTagVariablemapIter!= this->EssiTagVariableMap.end())   EssiTagVariablemapIter->second = stoi(newVar)+1;  
        }
    }

    MainFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);

    cout << "Sucessfully Converted" << endl;

    MainFile.close();
}

void GmshTranslator::ContactCommand(const int&i, const int& j){

    ofstream GeometryFile(geometryFile,ios::app);
    vector<string> Variables = VariableList.at(j);

    double tolerence = stof(Variables.at(0)); 
    int node1=0,node2=0;

    Tokenizer tkr = Tokenizer(Variables.at(1),"#");
    string type1 = this->delSpaces(tkr.nextToken());
    string tag1 = this->delSpaces(tkr.nextToken());

    tkr.set(Variables.at(2),"#");
    string type2 = this->delSpaces(tkr.nextToken());
    string tag2 = this->delSpaces(tkr.nextToken());

    map<int,NodeElement>::iterator Iterator1;
    map<int,NodeElement>::iterator Iterator2;

    if(!type1.compare("Enty")) Iterator1 = this->EntityMap.find(stoi(tag1));
    else if(!type1.compare("Phy")) Iterator1 = this->PhysicalGroupMap.find(stoi(tag1));

    if(!type1.compare("Enty")) Iterator2 = this->EntityMap.find(stoi(tag2));
    else if(!type1.compare("Phy")) Iterator2 = this->PhysicalGroupMap.find(stoi(tag2));

    map<int,int>::iterator Iterator1Begin = Iterator1->second.NodeList.begin();
    map<int,int>::iterator Iterator1End = Iterator1->second.NodeList.end();
    map<int,int>::iterator Iterator2Begin = Iterator2->second.NodeList.begin();
    map<int,int>::iterator Iterator2End = Iterator2->second.NodeList.end();
    map<int,Node> ::iterator NodeMap1, NodeMap2;

    for(map<int,int>::iterator It1 = Iterator1Begin; It1!=Iterator1End ;++It1){

        node1=It1->second; node2=0;

        for(map<int,int>::iterator It2 = Iterator2Begin; It2!=Iterator2End ;++It2){

            NodeMap1 = this->NodeMap.find(It1->second);
            NodeMap2 = this->NodeMap.find(It2->second);

            double modx = abs(NodeMap1->second.getXcord()-NodeMap2->second.getXcord());
            double mody = abs(NodeMap1->second.getYcord()-NodeMap2->second.getYcord());
            double modz = abs(NodeMap1->second.getZcord()-NodeMap2->second.getZcord());

            if(modx <=tolerence && mody <=tolerence && modz<=tolerence){

                node2=It2->second;
                break;
            }
        }

        if(node1!=0 && node2!=0){

            string str = "element";
            TempVariable.push(this->getVariable(str));
            TempVariable.push(to_string(node1));
            TempVariable.push(to_string(node2));
            TempVariable.push(Variables.at(3));
            TempVariable.push(Variables.at(4));
            TempVariable.push(Variables.at(5));
            TempVariable.push(Variables.at(6));
            TempVariable.push(Variables.at(7));
            TempVariable.push(Variables.at(8));
            TempVariable.push(Variables.at(9));

            GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
        }
    }

    cout << "Sucessfully Converted" << endl;

    GeometryFile.close();
}

void GmshTranslator::MaterialVariationalCommand(const int&i, const int& j){

    // cout << "Material Variational Command - A type of Elemental Command";

    map<int,NodeElement>::iterator PhysicalGroupMapIter =this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
    map<string,int>::iterator MaterialTagIter;
    map<string,Semantics>::iterator TempFunctionIter;
    
    ofstream GeometryFile(geometryFile, ios::app);
    ofstream MainFile(mainFile, ios::app);

    GeometryFile << "\n//*************************************************************************************************************************\n";
    GeometryFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Begins\n";  
    GeometryFile << "//*************************************************************************************************************************\n\n";

    if(PhysicalGroupMapIter==this->PhysicalGroupMap.end() || PhysicalGroupMapIter->second.ElementList.size()==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " is not executed as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
    map<int,int>  NodeList =PhysicalGroupMapIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = this->NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
     
    int nofRun=0;

    string gmshCommandtag = Variables.at(0);
    string arguments = Variables.at(1); replace( arguments.begin(), arguments.end(), ';', ',' );

    string ElementalCommand =  gmshCommandtag +"{1,"+arguments+"}";

    //cout<<endl<<ElementalCommand<< " "<<NofEssiVariables<< endl;

    PhysicalGroup TempPhyGroup = PhysicalGroup(); TempPhyGroup.Process(ElementalCommand);
    TempFunctionIter = this->FunctionMap.find(TempPhyGroup.getCommandList().at(0));
    OctParser valuate = OctParser();

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=2, MatTag=0;
        // cout << endl<< ElementalCommand << " " << TempFunctionIter->second.getElementId() <<endl;

        //running script in octave

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

            // cout << NofEssiVariables << " " << Variables.size() << endl;

            for(int l=0 ; l<NofEssiVariables ;l++ ){

                if (!EssiVariables.at(l).compare("material#1")){
                   string ScriptFunction = "material";
                   this->TempVariable.push(this->getVariable(ScriptFunction));
                }
                else{

                    string ScriptFunction = Variables.at(n++);
                    string function = ScriptVariables + ScriptFunction + ";";
                    string prec = Variables.at(n++) ;
                    // cout << function << "->" << endl;;// << "  ";
                    // cout << valuate.eval(function) << endl;
                    // // cout << to_string(roundToSignificantFigures(stof(valuate.eval(function)), stoi(Variables.at(n++))));
                    string var = to_string(roundToSignificantFigures(stof(valuate.eval(function)), stoi(prec)));
                    Material = Material + " " + var;
                    this->TempVariable.push(var);  
                }
            }
            //cout << endl << "-----------------------------" << endl;;
            //cout << Material<< endl;

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

            ElementalCommand = gmshCommandtag +"{"+to_string(MatTag)+","+arguments+"}";
            // cout << ElementalCommand;
            PhysicalGroup ElemPhyGroup = PhysicalGroup(); ElemPhyGroup.Process(ElementalCommand);
            TempFunctionIter = this->FunctionMap.find(ElemPhyGroup.getCommandList().at(0));

            // cout << "size()" << ElemPhyGroup.getVariableList().size();

            vector<string> NewVariables = ElemPhyGroup.getVariableList().at(0);
            vector<string> NewEssiVariables= TempFunctionIter->second.getVarList();
            int NewNofEssiVariables = TempFunctionIter->second.getNofEssiVariables();

           // cout << "NewVariableSize" << NewVariables.size() << " NewEssiVariablesSize" << NewEssiVariables.size()<<endl;

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
                else {
                    this->TempVariable.push(NewVariables.at(n++));   
                }
            }

            GeometryFile << this->PrintEssiCommand(TempFunctionIter->second.getEssiCommand(),TempFunctionIter->second.getNofEssiVariables(),j);
        }
    }

    if(nofRun==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " could not find any nodes/elements on which it operates" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    cout << "Sucessfully Converted" << endl;

    GeometryFile << "\n//*************************************************************************************************************************\n";
    GeometryFile << "//\t\t\t\t\t\t\t" <<  this->UserCommandList.at(j) << "Ends\n";  
    GeometryFile << "//*************************************************************************************************************************\n\n";

    remove( "script" );
    MainFile.close();
    GeometryFile.close();
}

void GmshTranslator::DRMCommand(const int&i, const int& j){

    // Add Node Command
    vector<string> Variables = VariableList.at(j);
    int nofRun=0;
    string NodesFileName = this->pwd +"_" + VariableList.at(j).at(0) +"_Nodes.txt";
    string ElementsFileName = this->pwd +"_" + VariableList.at(j).at(0) +"_Elements.txt";
    ofstream NodesFile(NodesFileName,ios::out); 
    ofstream ElementsFile(ElementsFileName,ios::out); 

    map<int,NodeElement>::iterator PhysicalGroupMapIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());

    if(PhysicalGroupMapIter==this->PhysicalGroupMap.end()|| PhysicalGroupMapIter->second.NodeList.size()==0){

        string msg = "\033[1;33mWARNING:: The command \'" + this->UserCommandList.at(j) + "\'" + " failed to convert as there is no elements/nodes in the Physical Group" + " \033[0m\n" ; 
        cout << msg;
        return;        
    }

    vector<Element> ElementList = PhysicalGroupMapIter->second.ElementList;
    int ElementListSize = ElementList.size();

    for(int i=0; i<ElementListSize ; i++){
        
        ElementsFile << ElementList.at(i).getId() << "\t";
        int size =  ElementList.at(i).getNodeList().size();

        for( int j =0 ;j<size ; j++ )            
            ElementsFile << ElementList.at(i).getNodeList().at(j) << "\t";
        ElementsFile << "\n";
    }

    map<int,int> NodeList =PhysicalGroupMapIter->second.NodeList;
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();

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

    int nofTokens = 0;
    string Ecommand = "";
    Tokenizer inpString = Tokenizer(Command,"{}") ;

    // cout <<  this->TempVariable.size() << " "  <<  NofEssiVariables << endl;
    if(this->TempVariable.size() > NofEssiVariables) {

        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + "has more than required variables" + " \033[0m\n" ; 
        throw msg.c_str();
    }

    if(this->TempVariable.size() < NofEssiVariables) {
        
        string msg = "\033[1;31mERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + "has less than required variables" + " \033[0m\n" ; 
        throw msg.c_str();
    }
    
    while(inpString.hasMoreTokens() && nofTokens++<NofEssiVariables){

        Ecommand = Ecommand + inpString.nextToken() + "\t";
        Ecommand = Ecommand + TempVariable.front() + " ";
        TempVariable.pop();
    }
    
    Ecommand = Ecommand + " " + inpString.nextToken() + "\n";

    return Ecommand;
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
