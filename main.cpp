// testing Tokenizer class

#include "Tokenizer.h"
#include "Semantics.h"
#include <iostream>

int main(int argc, char* argv[]){

    Semantics comm = Semantics("BeamElasticLumpedMass{ a, b, (s,d,e),(e) }","add element #{} type beam_elastic_lumped_mass with nodes ({},{}) cross_section={} elastic_modulus={} shear_modulus={} torsion_Jx={} beding_Iy={} bending_Iz={} mass_density={}  xz_plane_vector=({},{},{}) joint_1_offset=({},{},{}) joint_2_offset=({},{},{});", 10);
    cout << comm.getEssiTag() << endl;;
    cout << comm.getNofEssiVariables() << endl;
    cout << comm.getNofGmshVariables() << endl;

    // for (vector<string>::iterator i = comm.getTagList().begin() ; i<comm.getTagList().end();i++)
    //         cout << *i << "  ";
    // cout << endl;
    // cout << "EssiTagList" << comm.getEssiTagList().size()<<endl;
    // for (set<string>::iterator i = comm.getEssiTagList().begin() ; i!=comm.getEssiTagList().end(); ++i)
    //         cout << *i << ";  ";
    // cout << endl;

    queue<string> s = comm.getVarList();
    while(!s.empty())
    {
        string top = s.front();
        cout << top << endl;
        s.pop();
    }
    // if(comm.getMatchMode())
    //     cout << "true" << endl;
    // else
    //      cout << "false" << endl;;

    
    Tokenizer str= Tokenizer("!sumeet","!");
    cout << str.nextToken() << endl;;
    // string token;
    // vector<string> strList;
    // int counter = 0;

    // str.set("This is a class work sample");
    // string::iterator i = str.getString().begin() ;
    
    // str.setMode(1);
    // str.setcurrPos(str.getString().length()-1);
    // strList=str.split();
    // cout <<strList.size();

    // for(vector<string>::iterator i=strList.begin();i<strList.end();i++)
    //         cout<< *i << endl;
    // cout << endl;
    

    return 0;
}

