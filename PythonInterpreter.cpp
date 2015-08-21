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
#include <cmath>
#include "PythonInterpreter.h"
#include "Node.h"

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
	return;
}


/*******************************************************************************
********************************* Public Function ******************************
*******************************************************************************/

void PythonInterpreter::loadMshFile(const string& mshFile,int override){

	ConvertFile(mshFile,override);
	return;
}

void PythonInterpreter::Convert(const string& GmssiCommand){

	Translator.Convert(trim(GmssiCommand));
	return;
}

int PythonInterpreter::getEssiTag(const string& EssiTag){

	string Tag = EssiTag;
	string newNumber = Translator.getVariable(Tag);
	return stoi(newNumber);
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

SelectionData PythonInterpreter::BoxSelection(string PhysEntyTag, double x1,double x2,double y1,double y2,double z1,double z2){

	vector<Element> ElementList;

	// Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    if(!setTypeIter(TypeIter,PhysEntyTag))
    	ElementList = TypeIter->second.ElementList;
    else
    	ElementList= this->Translator.GmshParse.getElementList();
	/// Ends Initialization here

	struct SelectionData newSelectionData;
	int ElementListSize = ElementList.size();
	set<int> TempNodeList;

	for (int i =0 ; i< ElementListSize ; i++){

		Element NewElement = ElementList.at(i);
		vector<int> Nodelist = NewElement.getNodeList();
		int NodelistSize = Nodelist.size(), NofNodesMatched = 0;

		for(int j=0 ; j<NodelistSize ; j++ ){

			Node node = Translator.NodeMap.find(Nodelist.at(j))->second;
			double x = node.getXcord(), y = node.getYcord(), z = node.getZcord(); 

			if(x>=x1 && x<=x2 && y>=y1 && y<=y2 && z>=z1 && z<=z2){

				TempNodeList.insert(node.getId());
				NofNodesMatched++;
			}
		}

		if(NofNodesMatched==NodelistSize)
			newSelectionData.ElementList.push_back(NewElement);	
	}

	for (set<int>::iterator it=TempNodeList.begin(); it!=TempNodeList.end(); ++it)
		newSelectionData.NodeList.push_back(Translator.NodeMap.find(*it)->second);

	return newSelectionData;
}

SelectionData PythonInterpreter::SphereSelection(string PhysEntyTag,double radius,double center_x,double center_y,double center_z){

	vector<Element> ElementList;

	// Checking the tags and initiallizing whether Phy or Enty Tag or nothing
    map<int,NodeElement>::iterator TypeIter;
    if(!setTypeIter(TypeIter,PhysEntyTag))
    	ElementList = TypeIter->second.ElementList;
    else
    	ElementList= this->Translator.GmshParse.getElementList();
	/// Ends Initialization here

	struct SelectionData newSelectionData;
	int ElementListSize = ElementList.size();
	set<int> TempNodeList;

	for (int i =0 ; i< ElementListSize ; i++){

		Element NewElement = ElementList.at(i);
		vector<int> Nodelist = NewElement.getNodeList();
		int NodelistSize = Nodelist.size(), NofNodesMatched = 0;

		for(int j=0 ; j<NodelistSize ; j++ ){

			Node node = Translator.NodeMap.find(Nodelist.at(j))->second;
			double x = node.getXcord(), y = node.getYcord(), z = node.getZcord(); 

			if(sqrt((x-center_x)*(x-center_x)+(y-center_y)*(y-center_y)+(z-center_z)*(z-center_z))<=radius){

				TempNodeList.insert(node.getId());
				NofNodesMatched++;
			}
		}

		if(NofNodesMatched==NodelistSize)
			newSelectionData.ElementList.push_back(NewElement);	
	}

	for (set<int>::iterator it=TempNodeList.begin(); it!=TempNodeList.end(); ++it)
		newSelectionData.NodeList.push_back(Translator.NodeMap.find(*it)->second);

	return newSelectionData;
}

void PythonInterpreter::CreatePhysicalGroup (string Name,vector<Node> NodeList, vector<Element> ElementList){

	NodeElement newNodeElement; 
	map<int,int> NodeNumberNodeMap;
	int ElementListSize=ElementList.size(), NodelistSize=NodeList.size(), newPhysicalGroupTag = this->Translator.PhysicalGroupMap.size()+1;


	for(int i=0 ; i<ElementListSize ;i++){
		ElementList.at(i).setPhysicalTag(newPhysicalGroupTag);
		ElementList.at(i).setEntityTag(this->Translator.NewEntity++);
		string str = "element";
		ElementList.at(i).setId(stoi(this->Translator.getVariable(str)));
		this->Translator.GmshParse.addElement(ElementList.at(i));
	}

	for(int i=0 ; i<NodelistSize ;i++)
		NodeNumberNodeMap.insert(pair<int,int>(NodeList.at(i).getId(),NodeList.at(i).getId()));

	newNodeElement.ElementList = ElementList;
	newNodeElement.NodeList = NodeNumberNodeMap;

    this->Translator.PhysicalGroupMap.insert(pair<int,NodeElement>(newPhysicalGroupTag,newNodeElement));
    string PhysicDes = "3 "+ to_string(newPhysicalGroupTag)+  " \"$NewPhysicalGroup_" + to_string(newPhysicalGroupTag) +  "$\"";
    cout << "\033[1;36mNew Physical Group " << newPhysicalGroupTag << " created "  << "\033[0m\n";
    PhysicalGroup newPhysicalGroup = PhysicalGroup(PhysicDes);
    this->Translator.PhysicalGroupList.push_back(newPhysicalGroup);

	return;
}

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

string PythonInterpreter::getFile(){

	return Translator.GmshFile;
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

int PythonInterpreter::setTypeIter(map<int,NodeElement>::iterator &TypeIter,const string& variable){

    Tokenizer tknzr = Tokenizer(variable," #");

    if(tknzr.countTokens()==2){

        string type = delSpaces(tknzr.nextToken());
        int tag = stoi(delSpaces(tknzr.nextToken()));

        if(!type.compare("Enty")){

            TypeIter = this->Translator.EntityMap.find(tag);

            if(TypeIter==this->Translator.EntityMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command failed to convert as there is no elements/nodes in the Physical Group \033[0m\n" ; 
                cout << msg;
                return 0 ;        
            }
        }

        else if (!type.compare("Phy")){

            TypeIter = this->Translator.PhysicalGroupMap.find(tag);

            if(TypeIter==this->Translator.PhysicalGroupMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command failed to convert as there is no elements/nodes in the Physical Group \033[0m\n" ; 
                cout << msg;
                return 0 ;        
            }
        }

        else{

            string msg = "\033[1;31mERROR:: The command has a syntaxERROR in Enty/Phy# tag \033[0m\n" ;
            throw msg.c_str();
        }
    }

    else{

        return 1;
    }

    return 0;
}

string PythonInterpreter::delSpaces(string str){

   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}

string PythonInterpreter::trim(const string& str, const string& delimiters ){
	string s=str;
    s.erase( s.find_last_not_of( delimiters ) + 1 ).erase( 0, s.erase( s.find_last_not_of( delimiters ) + 1 ).find_first_not_of( delimiters ) );
    return s;
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
    class_<SelectionData>("SelectionData")
	    .def_readonly("NodeList",&SelectionData::NodeList)
	    .def_readonly("ElementList",&SelectionData::ElementList);

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

    class_<std::vector<int> >("NodeNumberList")
        .def(vector_indexing_suite<std::vector<int>>());

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
    	.def("SphereSelection",&PythonInterpreter::SphereSelection)
    	.def("UpdateGmshFile",&PythonInterpreter::UpdateGmshFile)
    	.def("BoxSelection",&PythonInterpreter::BoxSelection)
    	.def("CreatePhysicalGroup",&PythonInterpreter::CreatePhysicalGroup);
}