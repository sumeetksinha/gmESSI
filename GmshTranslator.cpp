/*****************************************************************************
*                               GmshTranslator.cpp
*                               =================
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/


#include "GmshTranslator.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

using namespace::std;


/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

GmshTranslator::GmshTranslator(){}

GmshTranslator::GmshTranslator(const string& gmshFile, const string& newDir){

    GmshFile = gmshFile+ ".msh";

    geometryFile = newDir + gmshFile + "_geometry.fei";
    loadFile = newDir + gmshFile + "_load.fei";
    mainFile = newDir + gmshFile + "_analysis.fei";
}

GmshTranslator::GmshTranslator(const string& gmshFile, const string& mappingFile, const string& newDir){

    GmshFile = gmshFile + ".msh";
    MappingFile = mappingFile;

    geometryFile = newDir + gmshFile + "_geometry.fei";
    loadFile = newDir + gmshFile + "_load.fei";
    mainFile = newDir + gmshFile + "_analysis.fei";
}

GmshTranslator::~GmshTranslator(){}

/******************************************************************************
**************************** Public Functions *********************************
******************************************************************************/

void GmshTranslator::setGmshFile(const string& gmshFile, const string& newDir){

    this->GmshFile = gmshFile +".msh";
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

    ofstream MainFile(mainFile,ios::out);  MainFile.close();
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
    cout << "PhysicalGroupListSize " << PhysicalGroupListSize<< endl << endl;

    for (unsigned i=0; i<PhysicalGroupListSize; i++){

        this->CommandList = this->PhysicalGroupList.at(i).getCommandList();
        this->VariableList = this->PhysicalGroupList.at(i).getVariableList();
        this->NofVariablesList = this->PhysicalGroupList.at(i).getNofVariables();
        this->UserCommandList = this->PhysicalGroupList.at(i).getUserCommandList();
        int CommandListSize = CommandList.size();

        for (unsigned j=0; j<CommandListSize; j++){
            
            this->FunctionIter = this->FunctionMap.find(this->CommandList.at(j));
            cout << this->CommandList.at(j) << ";";

            if (this->FunctionIter != this->FunctionMap.end()){
                
                cout<< "Function Found so success!!" << endl;
                cout<< "Element Id !!" << this->FunctionIter->second.getElementId() <<endl;

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
            }
            else{
                
                cout << endl <<  "\033[1;33m WARNING:: Execuation of the command escaped. The essi command \'" << this->UserCommandList.at(j) << "\'" << "could not be found \n" << " \033[0m\n" ; 
            }
        }
    }
}

void GmshTranslator::ElementalCommand(const int& i, const int& j){

    map<int,NodeElement>::iterator PhysicalGroupMapIter =this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
    map<int,int>  NodeList =PhysicalGroupMapIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = this->NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
    ofstream GeometryFile(geometryFile, ios::app); 

    cout<< "Elemental Command" ;

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=0 ;

        if( !(this->FunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

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
                    this->TempVariable.push(Variables.at(n++));   
                }
            }

            GeometryFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
        }
    }

    GeometryFile.close();
}

void GmshTranslator::ElementalCompoundCommand(const int& i, const int& j){

    map<int,NodeElement>::iterator PhysicalGroupMapIter =this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
    map<int,int>  NodeList =PhysicalGroupMapIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = this->NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
    ofstream LoadFile(loadFile,ios::app); 

    cout<< "Elemental Compound Command";

    Tokenizer tkr = Tokenizer(Variables.at(0),"#");
    string type = this->delSpaces(tkr.nextToken());
    int tag = stoi(this->delSpaces(tkr.nextToken()));

    map<int,NodeElement>::iterator EntityMapIter = this->EntityMap.find(tag);
    map<int,NodeElement>::iterator PhysicalMapIter = this->PhysicalGroupMap.find(tag);

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=1 ;

        if( !(this->FunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

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

                            map<int,int>::iterator NodeIter = PhysicalMapIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));
                            map<int,int>::iterator NodeIterEnd = PhysicalMapIter->second.NodeList.end();

                            if(NodeIter!= NodeIterEnd){
                                this->TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
                                loop = false;
                            }

                            m++;
                        }

                        else if (!type.compare("Enty")){

                            map<int,int>::iterator NodeIter = EntityMapIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));
                            map<int,int>::iterator NodeIterEnd = EntityMapIter->second.NodeList.end();

                            if(NodeIter!= NodeIterEnd){
                               this-> TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
                                loop = false;
                            }
                            
                            m++;
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
            else
                this->clear(this->TempVariable);
        }
    }

    LoadFile.close();
}

void GmshTranslator::NodalCommand(const int& i, const int& j){

    cout<< "Nodal Commands ";

    map<int,NodeElement>::iterator PhysicalGroupMapIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
    map<int,int> NodeList =PhysicalGroupMapIter->second.NodeList;
    vector<string> Variables = this->VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = NofVariablesList.at(j);
    map<int,int> ::iterator NodeListBegin = NodeList.begin();
    map<int,int> ::iterator NodeListEnd = NodeList.end();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
    ofstream LoadFile(loadFile,ios::app); 

    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){

        int n=0 ;

        for(int l=0 ; l<NofEssiVariables ;l++ ){

            Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
            string var = tknzr.nextToken();
             
            if(!var.compare("node") || !var.compare("nodes")){
                this->TempVariable.push(to_string(it->second));  
            }
            else if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
                this->TempVariable.push(this->getVariable(var)); 
            }
            else {
                this->TempVariable.push(Variables.at(n++));   
            }
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    LoadFile.close();
}

void GmshTranslator::GeneralElementalCommand(const int& i, const int& j){

    cout<< "General Elemental Commands";

    map<int,NodeElement>::iterator PhysicalGroupMapIter = this->PhysicalGroupMap.find(this->PhysicalGroupList.at(i).getId());
    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = NofVariablesList.at(j);
    int ElementListSize = ElementList.size();
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
    ofstream LoadFile(loadFile,ios::app); 

    for(int k =0;k<ElementListSize ; k++){

        int m =0, n=0 ;

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
                this->TempVariable.push(Variables.at(n++));   
            }
        }

        LoadFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    }

    LoadFile.close();
}

void GmshTranslator::SingularCommand(const int& i, const int& j){

    cout<< "Singular Commands";

    vector<string> Variables = VariableList.at(j);
    vector<string> EssiVariables= this->FunctionIter->second.getVarList();
    int NofVariables = NofVariablesList.at(j);
    int NofEssiVariables = this->FunctionIter->second.getNofEssiVariables();
    int n=0;
    ofstream MainFile(mainFile,ios::app);  

    for(int l=0 ; l<NofEssiVariables ;l++ ){

        Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
        string var = tknzr.nextToken();
    
        if (this->EssiTagVariableMap.find(var) != this->EssiTagVariableMap.end()){
           this->TempVariable.push(this->getVariable(var));
        }
        else {
            this->TempVariable.push(Variables.at(n++));   
        }
    }

    MainFile << this->PrintEssiCommand(this->FunctionIter->second.getEssiCommand(),this->FunctionIter->second.getNofEssiVariables(),j);
    MainFile.close();
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

    if(this->TempVariable.size() > NofEssiVariables) {

        string msg = "\033[1;31m ERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + "has more than required variables" + " \033[0m\n" ; 
        throw msg.c_str();
    }
    if(this->TempVariable.size() < NofEssiVariables) {
        
        string msg = "\033[1;31m ERROR:: The command \'" + this->UserCommandList.at(j) + "\'" + "has less than required variables" + " \033[0m\n" ; 
        throw msg.c_str();
    }
    
    while(inpString.hasMoreTokens() && nofTokens++<NofEssiVariables){

        Ecommand = Ecommand + inpString.nextToken() + " ";
        Ecommand = Ecommand + TempVariable.front();
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