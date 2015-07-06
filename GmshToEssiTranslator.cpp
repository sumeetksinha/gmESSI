// testing Tokenizer class

#include "Tokenizer.h"
#include "Semantics.h"
#include "PhysicalGroup.h"
#include "Element.h"
#include "Mapping.h"
#include "GmshParser.h"
#include <iostream>
#include <queue>
#include <algorithm>


map<string,int> EssiTagVariableMap;
queue<string> TempVariable;

string getVariable(string& var);
string delSpaces(string str);
void PrintEssiCommand(string Command);

int main(int argc, char* argv[]){

    Mapping Map = Mapping("mapping1.fei");
    Map.createMapping();
    GmshParser GmshParse = GmshParser("Bar_1.msh");
    GmshParse.Parse();

    // vector<Node> NodeList = GmshParse.getNodeList();
    vector<PhysicalGroup> PhysicalGroupList = GmshParse.getPhysicalGroupList();
    // map<string,string> ElementMap = Map.getElementMap();
    map<string,Semantics> FunctionMap = Map.getFunction();
    set<string> EssiTagList = Map.getEssiTagList();
    map<string,Semantics>::iterator FunctionIter;
    map<int,NodeElement> NodeElementMap = GmshParse.getPhysicalGroupMap();
    // map<int,Node> NodeMap = GmshParse.getNodeMap();

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

        vector<string>   CommandList = PhysicalGroupList.at(i).getCommandList();
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
                    
                    cout<< "Elemental Command" ;
                    map<int,NodeElement>::iterator NodeElementMapIter = NodeElementMap.find(PhysicalGroupList.at(i).getId());
                    vector<Element> ElementList = NodeElementMapIter->second.ElementList;
                    map<int,int>  NodeList = NodeElementMapIter->second.NodeList;
                    vector<string> Variables = VariableList.at(j);
                    vector<string> EssiVariables= FunctionIter->second.getVarList();
                    int NofVariables = NofVariablesList.at(j);
                    int ElementListSize = ElementList.size();
                    int NofEssiVariables = FunctionIter->second.getNofEssiVariables();

                    if (!(FunctionIter->second.getSemanticsId().compare("c"))){

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

                                        bool loop= false;
                                        
                                        while(!loop){

                                            map<int,int>::iterator NodeIter = NodeList.find(ElementList.at(k).getNodeList().at(m));

                                            if(NodeIter!=NodeList.end()){

                                                loop=true;
                                                TempVariable.push(to_string(ElementList.at(k).getNodeList().at(m++)));  
                                            }
                                            else{

                                                m=m+1;
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

                                PrintEssiCommand(FunctionIter->second.getEssiCommand());
                                cout << endl;
                            }

                        }
                    }

                    else{

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

                                PrintEssiCommand(FunctionIter->second.getEssiCommand());
                                cout << endl;
                            }

                        }

                    }
                }

                else if (!FunctionIter->second.getElementId().compare("n")){
                    
                    cout<< "Node Commands ";

                    map<int,NodeElement>::iterator NodeElementMapIter = NodeElementMap.find(PhysicalGroupList.at(i).getId());
                    map<int,int> NodeList = NodeElementMapIter->second.NodeList;
                    vector<string> Variables = VariableList.at(j);
                    vector<string> EssiVariables= FunctionIter->second.getVarList();
                    int NofVariables = NofVariablesList.at(j);
                    map<int,int> ::iterator NodeListBegin = NodeList.begin();
                    map<int,int> ::iterator NodeListEnd = NodeList.end();
                    int NofEssiVariables = FunctionIter->second.getNofEssiVariables();

                    cout << NofEssiVariables << endl;

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
                        PrintEssiCommand(FunctionIter->second.getEssiCommand());
                        cout << endl;
                    }
                }

                else if (!FunctionIter->second.getElementId().compare("ec")){
                    
                    cout<< "General Elemental Commands";

                    map<int,NodeElement>::iterator NodeElementMapIter = NodeElementMap.find(PhysicalGroupList.at(i).getId());
                    vector<Element> ElementList = NodeElementMapIter->second.ElementList;
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

                        PrintEssiCommand(FunctionIter->second.getEssiCommand());
                        cout << endl;

                    }
                }

                else if (!FunctionIter->second.getElementId().compare("sc")){

                    cout<< "General Elemental Commands";

                    vector<string> Variables = VariableList.at(j);
                    vector<string> EssiVariables= FunctionIter->second.getVarList();
                    int NofVariables = NofVariablesList.at(j);
                    int NofEssiVariables = FunctionIter->second.getNofEssiVariables();

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

                    PrintEssiCommand(FunctionIter->second.getEssiCommand());
                    cout << endl;
                }
            }
        }

        cout<< PhysicalGroupList.at(i).getId() << endl;
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

void PrintEssiCommand(string Command){

    int nofTokens = 0;
    string Ecommand = "";
    Tokenizer inpString = Tokenizer(Command,"{}") ;
    
    while(inpString.hasMoreTokens()){

        Tokenizer Var = Tokenizer(inpString.nextToken()," ,");

        if(!delSpaces(inpString.currToken()).compare(";")) break;                        // Termination Condition with ";"

        cout << inpString.currToken() << " ";
        cout << TempVariable.front();
        TempVariable.pop();
    }
    cout << " ;" << endl;
}


string delSpaces(string str){
   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}

