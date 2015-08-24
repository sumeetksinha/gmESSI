/********************************************************************************************************
*  File:        GmssiPython.h          	        | Copyright:: ##############################    *
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
#ifndef GMSSIPYTHON_H
#define GMSSIPYTHON_H

#include <string>
#include <vector>
#include "GmshTranslator.h"
#include "Element.h"
#include "Node.h"

struct SelectionData{
	
   vector<Element> ElementList;
   vector<Node> NodeList;
};

using namespace::std;

class GmssiPython{

	public:
		GmssiPython(); ~GmssiPython();
		GmssiPython(const string& mshFile, int override = 1);
		void loadMshFile(const string& mshFile, int override = 1);
		void loadMshFile2(const string& mshFile);
		void Convert(const string& GmssiCommand);
		int getEssiTag( const string& EssiTag);
		void DisplayNewTagNumbering();
		vector<Element> getPhysicalGroupElements(const int& n );
		vector<Node> getPhysicalGroupNodes(const int& n );
		vector<Element> getEntityGroupElements(const int& n );
		vector<Node> getEntityGroupNodes(const int& n );
		map<int,Node> getNodeMap();
		void ConvertFile(const string& mshFile,int override);
		void UpdateGmshFile();
		GmshTranslator Translator = GmshTranslator();
		SelectionData BoxSelection(string PhysEntyTag, double x1,double x2,double y1,double y2,double z1,double z2);
		SelectionData SphereSelection(string PhysEntyTag,double radius,double center_x,double center_y,double center_z);
		SelectionData getEntityGroupData(const int& tag);
		SelectionData getPhysicalGroupData(const int& tag);

		void CreatePhysicalGroup (string Name,vector<Node> NodeList, vector<Element> ElementList);
		string GmshFile="", pwd="", GeometryFile="", MainFile="", LoadFile="";

	private:

		string getFilePath();
		int setTypeIter(map<int,NodeElement>::iterator &TypeIter,const string& variable);
		string delSpaces(string str);
		string trim(const string& str, const string& delimiters = " []\f\n\r\t\v" );

};

#endif //GMSSIPYTHON_H