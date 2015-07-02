// testing Tokenizer class

#include "Tokenizer.h"
#include "Semantics.h"
#include "PhysicalGroup.h"
#include <iostream>

int main(int argc, char* argv[]){

    Semantics comm = Semantics("BeamElasticLumpedMass{}","add element #{} type beam_elastic_lumped_mass with nodes ({},{}) cross_section={} elastic_modulus={} shear_modulus={} torsion_Jx={} beding_Iy={} bending_Iz={} mass_density={}  xz_plane_vector=({},{},{}) joint_1_offset=({},{},{}) joint_2_offset=({},{},{});", 10);
    PhysicalGroup Phy = PhysicalGroup("2 1 \" $Bar_1$ < sumeet{dsfsdfsd,sd ,f,f f,f e} >, <dferfer{}>, <gahd{}>\"");
    
    // cout << comm.getEssiTag() << endl;;
    cout << comm.getNofEssiVariables() << endl;
    cout << comm.getNofGmshVariables() << endl;
    cout << comm.getNofTagVariables() << endl;
    cout << comm.getEssiTag() << endl;
    cout << comm.getEssiCommand() << endl;
    cout << comm.getGmshCommand() << endl;
    // cout << comm.getNofTagVariables() << endl;

    // // cout << comm.getTagList().size();
    for (unsigned i=0; i<comm.getVarList().size(); i++)
    	cout << " " <<comm.getVarList().at(i)<< endl;

    // cout << comm.getEssiCommand() << endl;
    // cout << comm.getGmshCommand() << endl;
    //   for (std::map<string,int>::iterator it=comm.getTagList().begin(); it!=comm.getTagList().end(); ++it)
    // std::cout << it->first << " => " << it->second << '\n';
    // cout << endl;
    // cout << "EssiTagList" << comm.getEssiTagList().size()<<endl;
    // for (set<string>::iterator i = comm.getEssiTagList().begin() ; i!=comm.getEssiTagList().end(); ++i)
    //         cout << *i << ";  ";
    // cout << endl;

    // queue<string> s = comm.getVarList();
    // while(!s.empty())
    // {
    //     string top = s.front();
    //     cout << top << endl;
    //     s.pop();
    // }
    // // if(comm.getMatchMode())
    // //     cout << "true" << endl;
    // // else
    // //      cout << "false" << endl;;

    
    // Tokenizer str= Tokenizer("!sumeet","!");
    // cout << str.nextToken() << endl;;
    // // string token;
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
    }

    return 0;*/
    

    return 0;
}

