/********************************************************************************************************
*  File:        gmESSIPython.cpp          	        | Copyright:: ##############################    *
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
#include "gmESSIPython.h"
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

gmESSIPython::gmESSIPython(){}

gmESSIPython::gmESSIPython(const string& mshFile, int overwrite){

	ConvertFile(mshFile,overwrite);
}

gmESSIPython::~gmESSIPython(){

	this->Translator.DisplayNewTagNumbering();
	this->Translator.UpdateGmshFile();
	this->Translator.Evaluate.exit();
}

/*******************************************************************************
********************************* Public Function ******************************
*******************************************************************************/

void gmESSIPython::loadMshFile(const string& mshFile,int overwrite){

	ConvertFile(mshFile,overwrite);
	return;
}

void gmESSIPython::loadMshFile2(const string& mshFile){

	ConvertFile(mshFile,1);
	return;
}

void gmESSIPython::Convert(const string& GmssiCommand){

	try{
		Translator.Convert(trim(GmssiCommand));
	} 
	catch (const char* msg){cerr << msg << endl; exit(EXIT_FAILURE);}

	return;
}

int gmESSIPython::getEssiTag(const string& EssiTag){

	string Tag = EssiTag;
	string newNumber = Translator.getVariable(Tag);
	return stoi(newNumber);
}

void gmESSIPython::DisplayNewTagNumbering(){
	
	Translator.DisplayNewTagNumbering();
	return;
}

vector<Element> gmESSIPython::getPhysicalGroupElements(const int& tag ){

	map<int,NodeElement>::iterator PhysicalGroupIter = Translator.PhysicalGroupMap.find(tag);
	if(PhysicalGroupIter!=Translator.PhysicalGroupMap.end())
		return PhysicalGroupIter->second.ElementList;
	else return vector<Element> ();
}

vector<Element> gmESSIPython::getPhysicalGroupElements2(const string& PhysicalGroupTag ){

	map<string,int>::iterator tagiter = this->Translator.PhysicalStringNameToIdMap.find(PhysicalGroupTag);
	if(tagiter!=this->Translator.PhysicalStringNameToIdMap.end())
		return getPhysicalGroupElements(tagiter->second);
	else return vector<Element> ();

}

vector<Node> gmESSIPython::getPhysicalGroupNodes(const int& tag ){

	map<int,NodeElement>::iterator PhysicalGroupIter = Translator.PhysicalGroupMap.find(tag);
	if(PhysicalGroupIter!=Translator.PhysicalGroupMap.end()){
		map<int,int> NodeList = PhysicalGroupIter->second.NodeList; vector<Node> NodeVector;
		for(map<int,int>::iterator it= NodeList.begin(); it!=NodeList.end();++it)
			NodeVector.push_back(Translator.NodeMap.find(it->second)->second);
		return NodeVector;
	}
	else return vector<Node> ();
}

vector<Node> gmESSIPython::getPhysicalGroupNodes2(const string& PhysicalGroupTag ){

	map<string,int>::iterator tagiter = this->Translator.PhysicalStringNameToIdMap.find(PhysicalGroupTag);
	if(tagiter!=this->Translator.PhysicalStringNameToIdMap.end())
		return getPhysicalGroupNodes(tagiter->second);
	else return vector<Node> ();

}

vector<Node> gmESSIPython::getEntityGroupNodes(const int& tag ){

	map<int,NodeElement>::iterator EntityGroupIter = Translator.EntityMap.find(tag);
	if(EntityGroupIter!=Translator.EntityMap.end()){
		map<int,int> NodeList = EntityGroupIter->second.NodeList;vector<Node> NodeVector;
		for(map<int,int>::iterator it= NodeList.begin(); it!=NodeList.end();++it)
			NodeVector.push_back(Translator.NodeMap.find(it->second)->second);
		return NodeVector;
	}
	else return vector<Node> ();
}

vector<Element> gmESSIPython::getEntityGroupElements(const int& tag ){

	map<int,NodeElement>::iterator EntityGroupIter = Translator.EntityMap.find(tag);
	if(EntityGroupIter!=Translator.EntityMap.end())
		return EntityGroupIter->second.ElementList;
	else return vector<Element> ();
}

// map<int,int> gmESSIPython::getPhysicalGroupNodes(const int& tag ){

// 	map<int,NodeElement>::iterator PhysicalGroupIter = Translator.PhysicalGroupMap.find(tag);
// 	if(PhysicalGroupIter!=Translator.PhysicalGroupMap.end()){
// 		return PhysicalGroupIter->second.NodeList;
// 	}
// 	else return map<int,int> ();
// }

// map<int,int> gmESSIPython::getEntityGroupNodes(const int& tag ){

// 	map<int,NodeElement>::iterator EntityGroupIter = Translator.EntityMap.find(tag);
// 	if(EntityGroupIter!=Translator.EntityMap.end()){
// 		return EntityGroupIter->second.NodeList;
// 	}
// 	else return map<int,int>();
// }

SelectionData gmESSIPython::getGroupData(const string& PhysEntyTag){

	struct SelectionData newSelectionData;

	try{

		// Checking the tags and initiallizing whether Phy or Enty Tag or All
	    map<int,NodeElement>::iterator TypeIter;
	    if(!setTypeIter(TypeIter,PhysEntyTag)){
	    	newSelectionData.ElementList = TypeIter->second.ElementList;
	    	map<int,int> NodeList = TypeIter->second.NodeList;
			for(map<int,int>::iterator it= NodeList.begin(); it!=NodeList.end();++it)
				newSelectionData.NodeList.push_back(Translator.NodeMap.find(it->second)->second);
	    	cout << "\033[1;36mGroup Data retrieved for " << PhysEntyTag << "\033[0m\n";
	    }
	    else{
	    	newSelectionData.ElementList = this->Translator.GmshParse.getElementList();
	    	map<int,Node> ::iterator AllNodeBegin = this->Translator.NodeMap.begin();
       		map<int,Node> ::iterator AllNodeEnd = this->Translator.NodeMap.end();
       		for(map<int,Node>::iterator it=AllNodeBegin; it!=AllNodeEnd; ++it)  
            	newSelectionData.NodeList.push_back(it->second);
	   		cout << "\033[1;36mGroup Data retieved for All Model \033[0m\n";
	    }
		/// Ends Initialization here

	}catch (const char* msg){cerr << msg << endl;}

	return newSelectionData;
}

SelectionData gmESSIPython::BoxSelectionData(string PhysEntyTag, double x1,double x2,double y1,double y2,double z1,double z2){

	vector<Element> ElementList;

	try{

		cout << "Box_Selection_Initiated \t \t " ; 
		// Checking the tags and initiallizing whether Phy or Enty Tag or All
	    map<int,NodeElement>::iterator TypeIter;
	    if(!setTypeIter(TypeIter,PhysEntyTag)){
	    	ElementList = TypeIter->second.ElementList;
	    	if(ElementList.size()>0) cout << "\033[1;36mBox Selection Made over " << PhysEntyTag << " with"  << x1 << " < x_cord < " <<  x2 << " , " << y1 << " < y_cord < " <<  y2  << " and " << z1 << "< z_cord < " <<  z2 << "\033[0m\n";
	    }
	    else{
	    	ElementList= this->Translator.GmshParse.getElementList();
	   		if(ElementList.size()>0) cout << "\033[1;36mBox Selection Made over All Model with " << x1 << " < x_cord < " <<  x2 << " , " << y1 << " < y_cord < " <<  y2  << " and " << z1 << "< z_cord < " <<  z2 << "\033[0m\n";
	    }
		/// Ends Initialization here

	}catch (const char* msg){cerr << msg << endl;}

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

SelectionData gmESSIPython::SphereSelectionData(string PhysEntyTag,double radius,double center_x,double center_y,double center_z){

	vector<Element> ElementList;

	cout << "Sphere_Selection_Initiated \t \t " ; 

	try{

		// Checking the tags and initiallizing whether Phy or Enty Tag or All
	    map<int,NodeElement>::iterator TypeIter;
	    if(!setTypeIter(TypeIter,PhysEntyTag)){
	    	ElementList = TypeIter->second.ElementList;
	    	if(ElementList.size()>0) cout << "\033[1;36mSphere Selection Made over " << PhysEntyTag << " with radius " << radius  << " and center at " << center_x << " " << center_y << " " << center_z << " "  << "\033[0m\n";
	    }
	    else{
	    	ElementList= this->Translator.GmshParse.getElementList();
	    	if(ElementList.size()>0) cout << "\033[1;36mSphere Selection Made over All Model with radius " << radius  << " and center at " << center_x << " " << center_y << " " << center_z << " "  << "\033[0m\n";
	    }
		/// Ends Initialization here

	}catch (const char* msg){cerr << msg << endl;}

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

void gmESSIPython::CreatePhysicalGroup (string Name,vector<Node> NodeList, vector<Element> ElementList){

	NodeElement newNodeElement; 
	map<int,int> NodeNumberNodeMap; 
	int ElementListSize=ElementList.size(), EntityNo=this->Translator.NewEntity++, NodelistSize=NodeList.size(), newPhysicalGroupTag = this->Translator.PhysicalGroupMap.size();

	for(int i=0 ; i<ElementListSize ;i++){
		ElementList.at(i).setPhysicalTag(newPhysicalGroupTag);
		ElementList.at(i).setEntityTag(EntityNo);
		string str = "element";
		ElementList.at(i).setId(stoi(this->Translator.getVariable(str)));
		this->Translator.GmshParse.addElement(ElementList.at(i));
	}

	for(int i=0 ; i<NodelistSize ;i++)
		NodeNumberNodeMap.insert(pair<int,int>(NodeList.at(i).getId(),NodeList.at(i).getId()));

	newNodeElement.ElementList = ElementList;
	newNodeElement.NodeList = NodeNumberNodeMap;

    this->Translator.PhysicalGroupMap.insert(pair<int,NodeElement>(newPhysicalGroupTag,newNodeElement));
    string PhysicDes = "3 "+ to_string(newPhysicalGroupTag)+  " \"$" + Name +  "$\"";
    cout << "\033[1;36mNew Physical Group " << newPhysicalGroupTag << " with name \"$" << Name << "$\" created "  << "\033[0m\n";
    PhysicalGroup newPhysicalGroup = PhysicalGroup(PhysicDes);
    this->Translator.PhysicalGroupList.push_back(newPhysicalGroup);
    this->Translator.PhysicalStringNameToIdMap.insert(pair<string,int>(newPhysicalGroup.getPhysicTag(),newPhysicalGroup.getId()));
    this->Translator.PhysicalStringNameToIdMap.insert(pair<string,int>(to_string(newPhysicalGroup.getId()),newPhysicalGroup.getId()));
    
	return;
}

map<int,Node> gmESSIPython::getNodeMap(){

	return Translator.NodeMap;
}

void gmESSIPython::UpdateGmshFile(){

	Translator.UpdateGmshFile();
	return;
}

/*******************************************************************************
********************************* Private Function *****************************
*******************************************************************************/

string gmESSIPython::getFilePath(){

	char filePath[256];
	
	if(!GetCurrentDir(filePath, sizeof(filePath))){

		cout<<"** ERROR - Something went wrong, exiting...\n";
	}
	
	string FilePath(filePath);

	return FilePath;
}

void gmESSIPython::ConvertFile(const string& mshFile,int overwrite){

	try{

	    string gmshFile = mshFile;
	    fstream InputFile (gmshFile,fstream::in);

	    if(!InputFile.is_open()){ 

	    	string msg = "\033[1;31mERROR:: The program failed to open the file = " +  gmshFile + " \033[0m\n" ; 
	    	throw msg.c_str();
	    }

	    Tokenizer str = Tokenizer(gmshFile,"/ .");
	    str.setcurrPos(gmshFile.length()-1); str.setMode(1); str.nextToken();gmshFile = str.nextToken();
	    string newDirectory= getFilePath() + slash + gmshFile+ "_ESSI_Simulation";
	    gmshFile = mshFile;   

	    int n = 1;string tempDirectory = newDirectory;
	    while(!mkdir(newDirectory.c_str(),0777)==0){ 

	    	if(overwrite>=1){ 
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

	   	Translator = gmESSITranslator(gmshFile, newDirectory);
	    Translator.Convert();

	    this->GmshFile = this->Translator.GmshFile;
	    this->GeometryFile = this->Translator.geometryFile;
	    this->MainFile = this->Translator.mainFile;
	    this->LoadFile = this->Translator.loadFile;
	    // this->pwd = this->Translator.pwd;

	    Tokenizer tknzr = Tokenizer(this->Translator.pwd,slash);
 		tknzr.setcurrPos(this->Translator.pwd.length()-1);
 		tknzr.setMode(1); tknzr.nextToken();

 		while(tknzr.hasMoreTokens())
 			this->pwd= slash + tknzr.nextToken() + this->pwd;
 		this->pwd = this->pwd + slash;


	} catch (const char* msg){cerr << msg << endl;}

	return;

}

int gmESSIPython::setTypeIter(map<int,NodeElement>::iterator &TypeIter,const string& variable){

    Tokenizer tknzr = Tokenizer(trim(variable),"#");
    int tag;

    if(variable.compare("All")){

        string type = tknzr.nextToken();

        if(!type.compare("Entity_Group")){

			try{
                tag = stoi(tknzr.nextToken());
             } catch(exception e) {string msg = "\033[1;31mERROR:: The command  failed to convert as there s no such  Entity Group  \"" + variable  + "\" \033[0m\n" ; throw msg.c_str();    }
                
            TypeIter = this->Translator.EntityMap.find(tag);

            if(TypeIter==this->Translator.EntityMap.end()|| TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command failed to convert as there is no elements/nodes in the Physical Group \"" + variable + "\" \033[0m\n" ; 
                throw msg.c_str();
                return 0 ;        
            }
        }

        else if (!type.compare("Physical_Group")){

            map<string,int>::iterator PhysicalStringNameToIdMapIter;
            PhysicalStringNameToIdMapIter = this->Translator.PhysicalStringNameToIdMap.find(tknzr.nextToken());

            if(PhysicalStringNameToIdMapIter==this->Translator.PhysicalStringNameToIdMap.end()){
                string msg = "\033[1;31mERROR:: The command failed to convert as there is no such Physical Group \"" + variable + "\"\033[0m\n" ; 
                throw msg.c_str();    
            }

            tag = PhysicalStringNameToIdMapIter->second;
            TypeIter = this->Translator.PhysicalGroupMap.find(tag);

            if(TypeIter->second.NodeList.size()==0){
                string msg = "\033[1;33mWARNING:: The command failed to convert as there is no elements/nodes in the Physical Group \"" + variable + "\" \033[0m\n" ; 
                throw msg.c_str();     
            }
        }

        else{
            string msg = "\033[1;31mERROR:: The command has a syntaxERROR in Entity_Group/Physical_Group#Tag tag \"" + variable + "\" \033[0m\n" ;
            throw msg.c_str();
        }
    }

    else{

        return 1;
    }

    return 0;
}

void gmESSIPython::setLoadFile(const string& loadfile){

	this->Translator.loadFile= loadfile;
	LoadFile = loadfile;
}

void gmESSIPython::setGeometryFile(const string& geometryfile){
	this->Translator.geometryFile = geometryfile;
	GeometryFile = geometryfile;
}

void gmESSIPython::setMainFile(const string& mainfile){
	this->Translator.mainFile = mainfile;
	MainFile = mainfile;
}

string gmESSIPython::delSpaces(string str){

   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}

string gmESSIPython::trim(const string& str, const string& delimiters ){
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

BOOST_PYTHON_MODULE(gmessi)
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

    class_<gmESSIPython>("gmESSIPython")
    	.def(init<const std::string&,int>())
    	.def(init<const std::string&>())
    	.def_readonly("pwd",&gmESSIPython::pwd)
	    .def_readonly("GmshFile",&gmESSIPython::GmshFile)
	    .def_readonly("LoadFile",&gmESSIPython::LoadFile)
	    .def_readonly("GeometryFile",&gmESSIPython::GeometryFile)
	    .def_readonly("MainFile",&gmESSIPython::MainFile)
	    .def("setMainFile",&gmESSIPython::setMainFile)
	    .def("setGeometryFile",&gmESSIPython::setGeometryFile)
	    .def("setLoadFile",&gmESSIPython::setLoadFile)
    	.def("loadMshFile",&gmESSIPython::loadMshFile)
    	.def("loadMshFile",&gmESSIPython::loadMshFile2)
    	.def("Convert",&gmESSIPython::Convert) 
    	.def("getNewEssiTag",&gmESSIPython::getEssiTag) 
    	.def("getEntityGroupElements",&gmESSIPython::getEntityGroupElements)
    	.def("getEntityGroupNodes",&gmESSIPython::getEntityGroupNodes)
    	.def("getPhysicalGroupElements",&gmESSIPython::getPhysicalGroupElements)
    	.def("getPhysicalGroupNodes",&gmESSIPython::getPhysicalGroupNodes)
    	.def("getPhysicalGroupElements",&gmESSIPython::getPhysicalGroupElements2)
    	.def("getPhysicalGroupNodes",&gmESSIPython::getPhysicalGroupNodes2)
    	.def("getNodeMap",&gmESSIPython::getNodeMap)
    	.def("getSphereSelectionData",&gmESSIPython::SphereSelectionData)
    	.def("CreatePhysicalGroup",&gmESSIPython::CreatePhysicalGroup)
    	.def("UpdateGmshFile",&gmESSIPython::UpdateGmshFile)
    	.def("BoxSelectionData",&gmESSIPython::BoxSelectionData)
    	.def("getGroupData",&gmESSIPython::getGroupData)
    	.def("DisplayNewTagNumbering",&gmESSIPython::DisplayNewTagNumbering);
}