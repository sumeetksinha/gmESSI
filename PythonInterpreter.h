#ifndef PYTHONINTERPRETER_H
#define PYTHONINTERPRETER_H

#include <string>
#include <vector>
#include "GmshTranslator.h"
#include "Element.h"
#include "Node.h"

typedef vector<Node> NodeList;

using namespace::std;

class PythonInterpreter{

	public:
		PythonInterpreter();
		PythonInterpreter(const string& mshFile, int override = 1);
		void loadMshFile(const string& mshFile, int override = 1);
		void Convert(const string& GmssiCommand);
		string getEssiTag( const string& EssiTag);
		vector<Element> getPhysicalGroupElements(const int& n );
		vector<Node> getPhysicalGroupNodes(const int& n );
		vector<Element> getEntityGroupElements(const int& n );
		vector<Node> getEntityGroupNodes(const int& n );
		map<int,Node> getNodeMap();
		string getFilePath();
		bool copyFile(const string& Source, const string& Destination);
		string getFile();
		void ConvertFile(const string& mshFile,int override);
		void UpdateGmshFile();
		string MshFile;
		GmshTranslator Translator = GmshTranslator();

	private:

		// void ConvertFile(const string& mshFile,int override);
		// string MshFile;
		// GmshTranslator Translator = GmshTranslator();

};

#endif //PYTHONINTERPRETER_H