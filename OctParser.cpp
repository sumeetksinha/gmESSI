/********************************************************************************************************
*  File:        OctaveParser.cpp                        | Copyright:: ##############################    *
*  Description: Contains the magic of translation       | BOX 1505                                      *
*  Rev:         Version 1                               | 125 25 ALVSJO                                 *
*  Created:     June 28, 2015                           | SWEDEN                                        *
*  Author:      Sumeet Kumar Sinha                      | tel int 530-601-8271                          *
*  Email:       sumeet.kumar507@gmail.com               |                                               *
*                                                                                                       *
*  The copyright to the computer program(s) herein is the property of #######################, ######.  *
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha      *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which      *
*  the program(s) have been supplied.                                                                   *
********************************************************************************************************/


#include <octave/oct.h>
#include <octave/octave.h>
#include <octave/parse.h>
#include <octave/toplev.h>
#include <octave/ov.h>
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


