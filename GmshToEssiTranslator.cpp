// testing Tokenizer class

#include "Tokenizer.h"
#include "Semantics.h"
#include "PhysicalGroup.h"
#include "Element.h"
#include "Mapping.h"
#include "GmshParser.h"
#include <iostream>

int main(int argc, char* argv[]){

    // Semantics comm = Semantics("BeamElasticLumpedMass{}","add element #{} type beam_elastic_lumped_mass with nodes ({},{}) cross_section={} elastic_modulus={} shear_modulus={} torsion_Jx={} beding_Iy={} bending_Iz={} mass_density={}  xz_plane_vector=({},{},{}) joint_1_offset=({},{},{}) joint_2_offset=({},{},{});", 10);
    // PhysicalGroup Phy = PhysicalGroup("2 1 \" $ Bar_1  $ \"");
    // Node Nd = Node("10 0.02 0 -0.26");
    // Element Elm = Element("2647 3 2 3 14 2564 2588 2589 2565");
    
    cout<< "Hello"<< endl;

    Mapping Map = Mapping("mapping1.fei");
    Map.createMapping();
    GmshParser GmshParse = GmshParser("Bar_1.msh");
    GmshParse.Parse();

    vector<Node> NodeList = GmshParse.getNodeList();
    vector<Element> ElementList = GmshParse.getElementList();
    vector<PhysicalGroup> PhysicalGroupList = GmshParse.getPhysicalGroupList();
    map<string,string> ElementMap = Map.getElementMap();
    map<string,Semantics> Function = Map.getFunction();
    set<string> EssiTagList = Map.getEssiTagList();
    map<string,Semantics>::iterator FunctionIter;
    map<string,Semantics>::iterator ElementMapIter;
    
    int PhysicalGroupListSize = PhysicalGroupList.size();
    cout << "PhysicalGroupListSize " << PhysicalGroupListSize<< endl << endl;
    for (unsigned i=0; i<PhysicalGroupListSize; i++){

        vector<string>   CommandList = PhysicalGroupList.at(i).getCommandList();
        vector<vector<string>> VariableList = PhysicalGroupList.at(i).getVariableList();
        vector<int> NofVariables = PhysicalGroupList.at(i).getNofVariables();
        int CommandListSize = CommandList.size();

        for (unsigned j=0; j<CommandListSize; j++){
            
            FunctionIter = Function.find(CommandList.at(j));
            if (FunctionIter != Function.end())
                cout<< "success" << endl;
        }
        // cout<< CommandList.at(j) << endl;
    }




// /**GmshParser**/

    // cout << "NewNode " << GmshParse.getNewNode() << endl;
    // cout << "NewElement " << GmshParse.getNewElement() << endl;
    // cout << "NewPhysicalgroup " << GmshParse.getNewPhysicalGroup() << endl;
    // cout << "NodeList " << GmshParse.getNodeList().size()<< endl << endl;
    // for (unsigned i=0; i<GmshParse.getNodeList().size()-1; i++)
    //     cout << " " <<GmshParse.getNodeList().at(i).getId() << " " << GmshParse.getNodeList().at(i).getXcord() << " " << GmshParse.getNodeList().at(i).getYcord() << " " << GmshParse.getNodeList().at(i).getZcord() << endl;
    // cout << endl;

    // cout << "ElementList " << GmshParse.getElementList().size()<< endl << endl;
    // for (unsigned i=0; i<GmshParse.getElementList().size(); i++)
    //     cout << " " <<GmshParse.getElementList().at(i).getId() << " " << GmshParse.getElementList().at(i).getType() << " " << GmshParse.getElementList().at(i).getNofTag() << endl;

    // cout << "PhysicalGroupList " << GmshParse.getPhysicalGroupList().size()<< endl << endl;
    // for (unsigned i=0; i<GmshParse.getPhysicalGroupList().size(); i++)
    //     cout << " " <<GmshParse.getPhysicalGroupList().at(i).getPhysicTag() << endl;

    // cout << "PhysicalGroupMap" << GmshParse.getPhysicalGroupMap().size() << endl << endl;
    //  for (auto& x: GmshParse.getPhysicalGroupMap())
    //     cout << x.first << '\n';


// /**PhysicalGroup**/

    /*cout << "Id " << Phy.getId() << endl;
    cout << "Type " << Phy.getType() << endl;
    cout << "PhysicaTag " << Phy.getPhysicTag() << endl;
    
    cout << "command List " << Phy.getCommandList().size()<< endl << endl;
    for (unsigned i=0; i<Phy.getCommandList().size(); i++)
        cout << " " <<Phy.getCommandList().at(i)<< endl;
    cout << endl;
    cout << "NofVariables List " << Phy.getNofVariables().size()<< endl << endl;
    for (unsigned i=0; i<Phy.getNofVariables().size(); i++)
        cout << " " <<Phy.getNofVariables().at(i)<< endl;
    cout << endl;
    cout << "variable List " << Phy.getVariableList().size()<< endl << endl;
    for (unsigned i=0; i<Phy.getVariableList().size(); i++)
        for (unsigned j=0; j<Phy.getNofVariables().at(i); j++)
            cout << " " <<Phy.getVariableList().at(i).at(j)<< endl;*/
  
// /**Mapping**/

    /*cout << "ElementMap " << Map.getEssiTagList().size()<< endl << endl;
    for (unsigned i=0; i<Map.getEssiTagList().size(); i++)
        cout << " " <<Map.getEssiTagList().at(i)<< endl;
    cout << endl;

    cout << "ElementMap " << Map.getElementMap().size()<< endl << endl;
    for (auto& x: Map.getElementMap())
        cout << " " <<x.first << " " << x.second<< endl;
    cout << endl;

    cout << "Function " << Map.getFunction().size()<< endl << endl;
   for (auto& x: Map.getFunction())
        cout << " " <<x.first << " " << x.second.getEssiCommand()<< endl;
    cout << endl;*/

// /**Element**/

   /* cout << "Id " << Elm.getId() << endl;
    cout << "Type " << Elm.getType() << endl;
    cout << "NofTag " << Elm.getNofTag() << endl;
    
    cout << "TagList " << Elm.getTagList().size()<< endl << endl;
    for (unsigned i=0; i<Elm.getTagList().size(); i++)
        cout << " " <<Elm.getTagList().at(i)<< endl;
    cout << endl;

    cout << "NodeList " << Elm.getNodeList().size()<< endl << endl;
    for (unsigned i=0; i<Elm.getNodeList().size(); i++)
        cout << " " <<Elm.getNodeList().at(i)<< endl;
    cout << endl;*/
    


// /**Node**/

   /* cout << "Id " << Nd.getId() << endl;
    cout << "Xcord " << Nd.getXcord() << endl;
    cout << "Ycord " << Nd.getYcord() << endl;
    cout << "Zcord " << Nd.getZcord() << endl;*/

// /**Semantics**/

    /*cout << comm.getNofEssiVariables() << endl;
    cout << comm.getNofGmshVariables() << endl;
    cout << comm.getNofTagVariables() << endl;
    cout << comm.getEssiTag() << endl;
    cout << comm.getEssiCommand() << endl;
    cout << comm.getGmshCommand() << endl;

    // // cout << comm.getTagList().size();
    for (unsigned i=0; i<comm.getVarList().size(); i++)
     cout << " " <<comm.getVarList().at(i)<< endl;*/

    

// /**Semantics**/

   /* Tokenizer str;
    string token;
    int counter = 0;

    // set source string with default delimiters (space, tab, and newline char)
    str.set("This is a testing\t string.\n\nTry to modify it \t\t yourself.");

    cout << str.nextToken() << ": ";
    cout << str.currToken().length() << endl;
    cout << str.nextToken() << ": ";
    cout << str.currToken().length() << endl;
    cout << str.prevToken() << ": ";
    cout << str.currToken().length() << endl;
    cout << str.prevToken() << ": ";
    cout << str.currToken().length() << endl;

    str.setMode(1);
    str.setcurrPos(str.getString().length()-1);

    // Tokenizer::next() returns a next available token from source string
    // If it reaches EOS, it returns zero-length string, "".
    while((token = str.nextToken()) != "")
    {
        ++counter;
        cout << counter << ": " << token << " : " <<token.length() << endl;
    }
    cout << endl;


    // set a different string and delimiters(~, _)
    // Notice that space is not delimiter any more.
    counter = 0;
    str.set("Now,~the_delimiters~~~are__changed~to_tild  and  underscore.~", "~_");
    while((token = str.nextToken()) != "")
    {
        ++counter;
        cout << counter << ": " << token << endl;
    }*/
   

    return 0;
}




