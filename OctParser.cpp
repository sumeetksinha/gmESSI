/*****************************************************************************
*                               OctParser.cpp
*                               =================
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 28-06-2015
*******************************************************************************/


#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/toplev.h>
#include <ov.h>
#include "OctParser.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace::std;

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/

OctParser::OctParser(){
	char *oct_argv[3] = {(char*)"embeded",(char*)"-q",(char*)"--interactive"}; 
    octave_main(3,oct_argv,true);
}

OctParser::~OctParser(){}

/******************************************************************************
**************************** Public Functions *********************************
******************************************************************************/

string OctParser::eval(const string& function) {

    int status = 0;
    ofstream ScriptFile("script", ios::out);
    ScriptFile << function ;
    ScriptFile.close();
    source_file("script");
    //eval_string(function,false,status);
    octave_value f = eval_string("num2str(ans);",false,status);
    string val = f.string_value();
    return val; 
}


void OctParser::exit(){

	// clean_up_and_exit (1);
	// reset();

}


