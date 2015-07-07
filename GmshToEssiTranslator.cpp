
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
#include <algorithm>


map<string,int> EssiTagVariableMap;
queue<string> TempVariable;
fstream MainFile(), NodeFile(), ElementFile(), LoadFile();

string getVariable(string& var);
void clear( queue<string> &q );
string delSpaces(string str);
void PrintEssiCommand(string Command, int NofEssiVariables);

int main(int argc, char* argv[]){

    if( argc == 0 ){ cout << "Please Enter the Gmsh File "<< endl; exit(0);}
    string file = argv[1];
    fstream InputFile(file,fstream::in);
    if(!InputFile.is_open())
        cout << "ERROR:: The program failed to open the file" << endl; 

    Mapping Map = Mapping("mapping.fei");
    Map.createMapping();
    GmshParser GmshParse = GmshParser("Bar_1.msh");
    GmshParse.Parse();

    vector<PhysicalGroup> PhysicalGroupList = GmshParse.getPhysicalGroupList();
    map<string,Semantics> FunctionMap = Map.getFunction();
    set<string> EssiTagList = Map.getEssiTagList();
    map<string,Semantics>::iterator FunctionIter;
    map<int,NodeElement>PhysicalGroupMap = GmshParse.getPhysicalGroupMap();
    map<int,NodeElement> EntityMap = GmshParse.getEntityMap();
    map<int,Node> NodeMap = GmshParse.getNodeMap();

    EssiTagVariableMap.insert(pair<string,int>("element",GmshParse.getNewElement()));
    EssiTagVariableMap.insert(pair<string,int>("node",GmshParse.getNewNode()));
    EssiTagVariableMap.insert(pair<string,int>("nodes",GmshParse.getNewNode()));
    EssiTagVariableMap.insert(pair<string,int>("damping",1));
    EssiTagVariableMap.insert(pair<string,int>("displacement",1));
    EssiTagVariableMap.insert(pair<string,int>("field",1));
    EssiTagVariableMap.insert(pair<string,int>("load",1));
    EssiTagVariableMap.insert(pair<string,int>("material",1));
    EssiTagVariableMap.insert(pair<string,int>("motion",1));

    int PhysicalGroupListSize = PhysicalGroupList.size();
    cout << "PhysicalGroupListSize " << PhysicalGroupListSize<< endl << endl;

    for (unsigned i=0; i<PhysicalGroupListSize; i++){

        vector<string> CommandList = PhysicalGroupList.at(i).getCommandList();
        vector<vector<string>> VariableList = PhysicalGroupList.at(i).getVariableList();
        vector<int> NofVariablesList = PhysicalGroupList.at(i).getNofVariables();
        int CommandListSize = CommandList.size();

        for (unsigned j=0; j<CommandListSize; j++){
            
            FunctionIter = FunctionMap.find(CommandList.at(j));
            cout << CommandList.at(j) << ";";

            if (FunctionIter != FunctionMap.end()){
                
                cout<< "Function Found so success!!" << endl;
                 cout<< "Element Id !!" << FunctionIter->second.getElementId() <<endl;

                if(FunctionIter->second.getMatchMode()){
                    
                    map<int,NodeElement>::iterator PhysicalGroupMapIter =PhysicalGroupMap.find(PhysicalGroupList.at(i).getId());
                    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
                    map<int,int>  NodeList =PhysicalGroupMapIter->second.NodeList;
                    vector<string> Variables = VariableList.at(j);
                    vector<string> EssiVariables= FunctionIter->second.getVarList();
                    int NofVariables = NofVariablesList.at(j);
                    int ElementListSize = ElementList.size();
                    int NofEssiVariables = FunctionIter->second.getNofEssiVariables();

                    if (!(FunctionIter->second.getSemanticsId().compare("c"))){   // meaning Elemental Compound Commands

                        cout<< "Elemental Compound Command" ;
                        Tokenizer tkr = Tokenizer(Variables.at(0),"#");
                        string type = delSpaces(tkr.nextToken());
                        int tag = stoi(delSpaces(tkr.nextToken()));

                        // cout<< type << " " << tag << endl; 
                        // cout << "EntityMap Size = " << EntityMap.size() << endl;
                        // cout << "PhysicalmapSize = " << PhysicalGroupMap.size() << endl;
                        // cout << "No. of Essi Variables= " << FunctionIter->second.getNofEssiVariables() << endl;

                        map<int,NodeElement>::iterator EntityMapIter = EntityMap.find(tag);
                        map<int,NodeElement>::iterator PhysicalMapIter =PhysicalGroupMap.find(tag);

                        for(int k =0;k<ElementListSize ; k++){

                            int m =0, n=1 ;

                            if( !(FunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

                                for(int l=0 ; l<NofEssiVariables ;l++ ){

                                    Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                                    string var = tknzr.nextToken();
                                 
                                    if(!var.compare("element")){
                                        TempVariable.push(to_string(ElementList.at(k).getId()));
  
                                    }
                                    else if(!var.compare("node") || !var.compare("nodes")){

                                        bool loop=true; int loopMax =  ElementList.at(k).getNodeList().size();

                                        while(loop && m<loopMax){
                                             
                                            if(!type.compare("Phy")){

                                                map<int,int>::iterator NodeIter = PhysicalMapIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));
                                                map<int,int>::iterator NodeIterEnd = PhysicalMapIter->second.NodeList.end();

                                                if(NodeIter!= NodeIterEnd){

                                                TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
                                                loop = false;
                                                }

                                                m++;
                                            }

                                            else if (!type.compare("Enty")){

                                                map<int,int>::iterator NodeIter = EntityMapIter->second.NodeList.find(ElementList.at(k).getNodeList().at(m));
                                                map<int,int>::iterator NodeIterEnd = EntityMapIter->second.NodeList.end();

                                                if(NodeIter!= NodeIterEnd){

                                                TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m)));
                                                loop = false;
                                                }
                                                
                                                m++;
                                            }
                                        }
                                    }
                                    else if (EssiTagVariableMap.find(var) != EssiTagVariableMap.end()){
                                        TempVariable.push(getVariable(var)); 

                                    }
                                    else {
                                        TempVariable.push(Variables.at(n++)); 

                                   }
                                }

                                if (TempVariable.size() == FunctionIter->second.getNofEssiVariables())  
                                    PrintEssiCommand(FunctionIter->second.getEssiCommand(),FunctionIter->second.getNofEssiVariables());
                                else
                                    clear(TempVariable);
                            }

                        }
                    }

                    else{

                        cout<< "Elemental Command" ;

                        for(int k =0;k<ElementListSize ; k++){

                            int m =0, n=0 ;

                            if( !(FunctionIter->second.getElementId().compare(to_string(ElementList.at(k).getType()) ))){

                                for(int l=0 ; l<NofEssiVariables ;l++ ){

                                    Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                                    string var = tknzr.nextToken();
                                 
                                    if(!var.compare("element")){
                                        TempVariable.push(to_string(ElementList.at(k).getId()));
                                    }
                                    else if(!var.compare("node") || !var.compare("nodes")){
                                        TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
                                    }
                                    else if (EssiTagVariableMap.find(var) != EssiTagVariableMap.end()){
                                        TempVariable.push(getVariable(var)); 
                                    }
                                    else {
                                        TempVariable.push(Variables.at(n++));   
                                    }
                                }

                                PrintEssiCommand(FunctionIter->second.getEssiCommand(),FunctionIter->second.getNofEssiVariables());
                            }

                        }

                    }
                }

                else if (!FunctionIter->second.getElementId().compare("nc")){
                    
                    cout<< "Nodal Commands ";

                    map<int,NodeElement>::iterator PhysicalGroupMapIter =PhysicalGroupMap.find(PhysicalGroupList.at(i).getId());
                    map<int,int> NodeList =PhysicalGroupMapIter->second.NodeList;
                    vector<string> Variables = VariableList.at(j);
                    vector<string> EssiVariables= FunctionIter->second.getVarList();
                    int NofVariables = NofVariablesList.at(j);
                    map<int,int> ::iterator NodeListBegin = NodeList.begin();
                    map<int,int> ::iterator NodeListEnd = NodeList.end();
                    int NofEssiVariables = FunctionIter->second.getNofEssiVariables();

                    for(map<int,int>::iterator it=NodeListBegin; it!=NodeListEnd; ++it){

                        int n=0 ;

                        for(int l=0 ; l<NofEssiVariables ;l++ ){

                            Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                            string var = tknzr.nextToken();
                             
                            if(!var.compare("node") || !var.compare("nodes")){
                                TempVariable.push(to_string(it->second));  
                            }
                            else if (EssiTagVariableMap.find(var) != EssiTagVariableMap.end()){
                                TempVariable.push(getVariable(var)); 
                            }
                            else {
                                TempVariable.push(Variables.at(n++));   
                            }
                        }

                        PrintEssiCommand(FunctionIter->second.getEssiCommand(),FunctionIter->second.getNofEssiVariables());
                    }
                }

                else if (!FunctionIter->second.getElementId().compare("ec")){
                    
                    cout<< "General Elemental Commands";

                    map<int,NodeElement>::iterator PhysicalGroupMapIter =PhysicalGroupMap.find(PhysicalGroupList.at(i).getId());
                    vector<Element> ElementList =PhysicalGroupMapIter->second.ElementList;
                    vector<string> Variables = VariableList.at(j);
                    vector<string> EssiVariables= FunctionIter->second.getVarList();
                    int NofVariables = NofVariablesList.at(j);
                    int ElementListSize = ElementList.size();
                    int NofEssiVariables = FunctionIter->second.getNofEssiVariables();

                    for(int k =0;k<ElementListSize ; k++){

                        int m =0, n=0 ;

                        for(int l=0 ; l<NofEssiVariables ;l++ ){
    
                            Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                            string var = tknzr.nextToken();
                           
                            if(!var.compare("element")){
                                TempVariable.push(to_string(ElementList.at(k).getId()));
                            }
                            else if(!var.compare("node") || !var.compare("nodes")){
                                TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
                            }
                            else if (EssiTagVariableMap.find(var) != EssiTagVariableMap.end()){
                                TempVariable.push(getVariable(var)); 
                            }
                            else {
                                TempVariable.push(Variables.at(n++));   
                            }
                        }

                        PrintEssiCommand(FunctionIter->second.getEssiCommand(),FunctionIter->second.getNofEssiVariables());
                    }
                }

                else if (!FunctionIter->second.getElementId().compare("sc")){

                    cout<< "Singular Commands";

                    vector<string> Variables = VariableList.at(j);
                    vector<string> EssiVariables= FunctionIter->second.getVarList();
                    int NofVariables = NofVariablesList.at(j);
                    int NofEssiVariables = FunctionIter->second.getNofEssiVariables();

                    // // cout<< type << " " << tag << endl; 
                    // // cout << "EntityMap Size = " << EntityMap.size() << endl;
                    // // cout << "PhysicalmapSize = " << PhysicalGroupMap.size() << endl;
                    cout << "No. of Essi Variables= " << FunctionIter->second.getNofEssiVariables() << endl;
                    cout << "No. of EVariables= " << NofVariables<< endl;

                    int n=0;

                    for(int l=0 ; l<NofEssiVariables ;l++ ){
    
                        Tokenizer tknzr = Tokenizer(EssiVariables.at(l),"#");
                        string var = tknzr.nextToken();
                    
                        if (EssiTagVariableMap.find(var) != EssiTagVariableMap.end()){

                            TempVariable.push(getVariable(var));
                        }
                        else {

                            TempVariable.push(Variables.at(n++));   
                        }
                    }

                    PrintEssiCommand(FunctionIter->second.getEssiCommand(),FunctionIter->second.getNofEssiVariables());
                }
            }
        }

        // cout<< PhysicalGroupList.at(i).getId() << endl;
    }

    return 0;
}


string getVariable(string& var){

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

void PrintEssiCommand(string Command, int NofEssiVariables){

    int nofTokens = 0;
    string Ecommand = "";
    Tokenizer inpString = Tokenizer(Command,"{}") ;
    
    while(inpString.hasMoreTokens() && nofTokens++<NofEssiVariables){

        cout << inpString.nextToken() << " ";
        cout << TempVariable.front();
        TempVariable.pop();
    }
    cout << " " <<inpString.nextToken() << endl;
}


string delSpaces(string str){

   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}

void clear( queue<string> &q ){

   queue<string> empty;
   swap( q, empty );
}
