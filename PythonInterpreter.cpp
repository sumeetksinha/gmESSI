/********************************************************************************************************
*  File:        PythonInterpreter.cpp          	        | Copyright:: ##############################    *
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
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <algorithm>
#include "PythonInterpreter.h"

#ifdef _WIN32 
    #include <direct.h>
    #define GetCurrentDir _getcwd
    #define slash "\\"
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
	#define slash "/"
#endif

/******************************************************************************
****************************** Constructor ************************************
******************************************************************************/


PythonInterpreter::PythonInterpreter(){}

PythonInterpreter::PythonInterpreter(const string& mshFile, int override){

	ConvertFile(mshFile,override);
}


/*******************************************************************************
********************************* Public Function ******************************
*******************************************************************************/

void PythonInterpreter::loadMshFile(const string& mshFile,int override){

	ConvertFile(mshFile,override);
}

void PythonInterpreter::Convert(const string& GmssiCommand){

	Translator.Convert(GmssiCommand);
}

string PythonInterpreter::getEssiTag(const string& EssiTag){

	string Tag = EssiTag;
	string newNumber = Translator.getVariable(Tag);
	return newNumber;
}

vector<Element> PythonInterpreter::getPhysicalGroupElements(const int& tag ){

	map<int,NodeElement>::iterator PhysicalGroupIter = Translator.PhysicalGroupMap.find(tag);
	if(PhysicalGroupIter!=Translator.PhysicalGroupMap.end())
		return PhysicalGroupIter->second.ElementList;
	else return vector<Element> ();
}

vector<Element> PythonInterpreter::getEntityGroupElements(const int& tag ){

	map<int,NodeElement>::iterator EntityGroupIter = Translator.EntityMap.find(tag);
	if(EntityGroupIter!=Translator.EntityMap.end())
		return EntityGroupIter->second.ElementList;
	else return vector<Element> ();
}

// map<int,int> PythonInterpreter::getPhysicalGroupNodes(const int& tag ){

// 	map<int,NodeElement>::iterator PhysicalGroupIter = Translator.PhysicalGroupMap.find(tag);
// 	if(PhysicalGroupIter!=Translator.PhysicalGroupMap.end()){
// 		return PhysicalGroupIter->second.NodeList;
// 	}
// 	else return map<int,int> ();
// }

// map<int,int> PythonInterpreter::getEntityGroupNodes(const int& tag ){

// 	map<int,NodeElement>::iterator EntityGroupIter = Translator.EntityMap.find(tag);
// 	if(EntityGroupIter!=Translator.EntityMap.end()){
// 		return EntityGroupIter->second.NodeList;
// 	}
// 	else return map<int,int>();
// }

vector<Node> PythonInterpreter::getPhysicalGroupNodes(const int& tag ){

	map<int,NodeElement>::iterator PhysicalGroupIter = Translator.PhysicalGroupMap.find(tag);
	if(PhysicalGroupIter!=Translator.PhysicalGroupMap.end()){
		map<int,int> NodeList = PhysicalGroupIter->second.NodeList; vector<Node> NodeVector;
		for(map<int,int>::iterator it= NodeList.begin(); it!=NodeList.end();++it)
			NodeVector.push_back(Translator.NodeMap.find(it->second)->second);
		return NodeVector;
	}
	else return vector<Node> ();
}

vector<Node> PythonInterpreter::getEntityGroupNodes(const int& tag ){

	map<int,NodeElement>::iterator EntityGroupIter = Translator.EntityMap.find(tag);
	if(EntityGroupIter!=Translator.EntityMap.end()){
		map<int,int> NodeList = EntityGroupIter->second.NodeList;vector<Node> NodeVector;
		for(map<int,int>::iterator it= NodeList.begin(); it!=NodeList.end();++it)
			NodeVector.push_back(Translator.NodeMap.find(it->second)->second);
		return NodeVector;
	}
	else return vector<Node> ();
}



map<int,Node> PythonInterpreter::getNodeMap(){

	return Translator.NodeMap;
}


string PythonInterpreter::getFilePath(){

	char filePath[256];
	
	if(!GetCurrentDir(filePath, sizeof(filePath))){

		cout<<"** ERROR - Something went wrong, exiting...\n";
	}
	
	string FilePath(filePath);

	return FilePath;
}

bool PythonInterpreter::copyFile(const string& Source, const string& Destination){

   	ifstream src(Source, ios::binary);
   	ofstream dest(Destination, ios::binary);
    dest << src.rdbuf();
    return src && dest;
}

string PythonInterpreter::getFile(){

	return MshFile;
}

void PythonInterpreter::UpdateGmshFile(){

	Translator.UpdateGmshFile();
}

/*******************************************************************************
********************************* Private Function *****************************
*******************************************************************************/

void PythonInterpreter::ConvertFile(const string& mshFile,int override){

	try{

	    string gmshFile = mshFile;
	    fstream InputFile (gmshFile,fstream::in);

	    if(!InputFile.is_open()){ 

	    	string msg = "\033[1;31mERROR:: The program failed to open the file = " +  gmshFile + " \033[0m\n" ; 
	    	throw msg.c_str();
	    	exit(0);
	    }

	    Tokenizer str = Tokenizer(gmshFile,"/ .");
	    str.setcurrPos(gmshFile.length()-1); str.setMode(1); str.nextToken();gmshFile = str.nextToken();
	    string newDirectory= getFilePath() + slash + gmshFile+ "_Essi_Simulation";
	    gmshFile = mshFile;   

	    // cout << "GmshFile:: " << gmshFile << endl;
	    // cout << "newDirectory:: " << newDirectory << endl;
	    // return;

	    // if(!str.nextToken().compare("msh")==0){

	    // 	string msg = "\033[1;31mERROR:: The file does not have .msh extension \033[0m\n" ; 
	    // 	throw msg.c_str();
	    // }
	    int n = 1;string tempDirectory = newDirectory;
	    while(!mkdir(newDirectory.c_str(),0777)==0){ 

	    	if(override>=1){ 
	    		cout << "\033[1;33mWARNING::Directory Allready Present. The contents of the Folder may get changed \033[0m\n"; 
	    		break;
	    	}
	    	else{ 
	    		newDirectory = tempDirectory+"_"+to_string(n); 
	    		n=n+1;
	    	}
	    }
	    cout << "\033[1;36mMessage::Files converted to " << newDirectory << "  \033[0m\n" << endl; 
	    newDirectory  =newDirectory +slash;

	   	Translator = GmshTranslator(gmshFile, newDirectory);
	    Translator.Convert();

	} catch (const char* msg){cerr << msg << endl;}

}

/*******************************************************************************
*********************** BoostPython Wrapper Function ***************************
*******************************************************************************/

#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/iterator.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/def.hpp>
#include <boost/python/args.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/class.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/return_internal_reference.hpp>


using namespace boost::python;

BOOST_PYTHON_MODULE(gmssi)
{
    class_<Element>("Element")
    	.def("getId",&Element::getId)
    	.def("getType",&Element::getType)
    	.def("getEntityTag",&Element::getEntityTag) 
    	.def("getPhysicalTag",&Element::getPhysicalTag) 
    	.def("getNodeList",&Element::getNodeList);

    class_<Node>("Node")
    	.def("getId",&Node::getId)
    	.def("getXcord",&Node::getXcord)
    	.def("getYcord",&Node::getYcord) 
    	.def("getZcord",&Node::getZcord);

    class_<std::vector<Node> >("NodeList")
        .def(vector_indexing_suite<std::vector<Node>>());

    class_<std::vector<Element> >("ElementList")
        .def(vector_indexing_suite<std::vector<Element>>());

    class_<std::map<int,int> >("NodeNumberNodeMap")
        .def(map_indexing_suite<std::map<int,int> >() );

    class_<std::map<int,Node> >("NodeMap")
        .def(map_indexing_suite<std::map<int,Node> >() );

    class_<PythonInterpreter>("PythonInterpreter")
    	.def(init<const std::string&,int>())
    	.def("loadMshFile",&PythonInterpreter::loadMshFile)
    	.def("getFile",&PythonInterpreter::getFile)
    	.def("Convert",&PythonInterpreter::Convert) 
    	.def("getEssiTag",&PythonInterpreter::getEssiTag) 
    	.def("getEntityGroupElements",&PythonInterpreter::getEntityGroupElements)
    	.def("getEntityGroupNodes",&PythonInterpreter::getEntityGroupNodes)
    	.def("getPhysicalGroupElements",&PythonInterpreter::getPhysicalGroupElements)
    	.def("getPhysicalGroupNodes",&PythonInterpreter::getPhysicalGroupNodes)
    	.def("getNodeMap",&PythonInterpreter::getNodeMap)
    	.def("getFilePath",&PythonInterpreter::getFilePath)
    	.def("UpdateGmshFile",&PythonInterpreter::UpdateGmshFile);
}