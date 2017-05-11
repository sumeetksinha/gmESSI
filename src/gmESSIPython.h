/*********************************************************************************************************
*  File:        gmESSIPython.h          	        	| Copyright:: Version of Creative Commons License*
*  Description: Python Module for gmESSI       			| Boris Jeremic,                                 *
*  Rev:         Version 1                               | University of California Davis                 *
*  Created:     June 28, 2015                           | California                                     *
*  Author:      Sumeet Kumar Sinha                      | jeremic@ucdavis.edu                            *
*  Email:       sumeet.kumar507@gmail.com               |                                                *
*                                                                                                        *
*  The copyright to the computer program(s) herein is the property of Sumeet Kumar Sinha, Boris Jeremic  *
*  The program(s) may be used and/or copied only with the written permission of Sumeet Kumar Sinha       *
*  or in accordance with the terms and conditions stipulated in the agreement/contract under which       *
*  the program(s) have been supplied.                                                                    *
*********************************************************************************************************/
#ifndef gmESSIPython_H
#define gmESSIPython_H

#include <string>
#include <vector>
#include "gmESSITranslator.h"
#include "Element.h"
#include "Node.h"

struct SelectionData{
	
   vector<Element> ElementList;
   vector<Node> NodeList;
};

using namespace::std;

class gmESSIPython{

	public:
		gmESSIPython(); ~gmESSIPython();
		gmESSIPython(const string& mshFile, int overwrite = 1);
		void loadMshFile(const string& mshFile, int overwrite = 1);
		void loadMshFile2(const string& mshFile);
		void Convert(const string& GmssiCommand);
		int getEssiTag( const string& EssiTag);
		void DisplayNewTagNumbering();
		vector<Element> getPhysicalGroupElements(const int& n );
		vector<Element> getPhysicalGroupElements2(const string& n );
		vector<Node> getPhysicalGroupNodes(const int& n );
		vector<Node> getPhysicalGroupNodes2(const string& n );
		vector<Element> getEntityGroupElements(const int& n );
		vector<Node> getEntityGroupNodes(const int& n );
		map<int,Node> getNodeMap();
		void ConvertFile(const string& mshFile,int overwrite);
		void UpdateGmshFile();
		void setPrecision(int n);
		gmESSITranslator Translator = gmESSITranslator();
		SelectionData getGroupData(const string& PhysEntyTag);
		SelectionData BoxSelectionData(string PhysEntyTag, double x1,double x2,double y1,double y2,double z1,double z2);
		SelectionData SphereSelectionData(string PhysEntyTag,double radius,double center_x,double center_y,double center_z);
		void setLoadFile(const string& loadfile);
		void setGeometryFile(const string& GeometryFile);
		void setMainFile(const string& MainFile);

		/******************** Optimizing Functions for ESSI *******************************************/
		map<int,int> getNodeNoMap();
		map<int,int> getElementNoMap();
		void setNodeNoMap(map<int,int> NodeNoMap);
		void setElementNoMap(map<int,int> ElementNoMap);
		/*********************************************************************************************/

		int addNode(Node node);
		int addElement(Element ele);
		int getGmshTag(const string& EssiTag);
		int getNewPhysicalGroupTag();
		int getNewEntityTag();

		void CreatePhysicalGroup (string Name,vector<Node> NodeList, vector<Element> ElementList);
		string GmshFile="", pwd="", GeometryFile="", MainFile="", LoadFile="";

	private:

		string getFilePath();
		int setTypeIter(map<int,NodeElement>::iterator &TypeIter,const string& variable);
		string delSpaces(string str);
		string trim(const string& str, const string& delimiters = " []\f\n\r\t\v" );

};

#endif //gmESSIPython_H