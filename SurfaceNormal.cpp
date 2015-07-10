// /****************************************************************************
// * 								SurfaceNormal.cpp
// * 								=================
// *
// * AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
// * CREATED: 9-07-2015
// *******************************************************************************/

#include "SurfaceNormal.h"

// /******************************************************************************
// ****************************** Constructor ************************************
// ******************************************************************************/

SurfaceNormal::SurfaceNormal(){}

SurfaceNormal::SurfaceNormal(const vector<int> nodeList, const map<int,Node> nodeMap){

    this->NodeList = nodeList;
    this->NodeMap = nodeMap;
}

SurfaceNormal::~SurfaceNormal(){}

// /******************************************************************************
// **************************** Public Functions *********************************
// ******************************************************************************/

void SurfaceNormal::setNodeList(const vector<int> nodeList){

    this->NodeList = nodeList;
}

void SurfaceNormal::setNodeMap(const map<int,Node> nodeMap){

    this->NodeMap = nodeMap;
}

vector<double> SurfaceNormal::getNormalVector(){

	this->formNormalVector();
	return this->NormalVector;
}

// /******************************************************************************
// **************************** Private Functions ********************************
// ******************************************************************************/

void SurfaceNormal::formNormalVector(){

	Vector A,B,C,Norm, v1,v2;

	if ( this->NodeList.size()<3)throw ("cannot find the Normal Vector");

	map<int,Node>::iterator it;

	it = this->NodeMap.find(this->NodeList.at(0));
	A.x = it->second.getXcord();A.y = it->second.getYcord();A.z = it->second.getZcord();
	it= this->NodeMap.find(this->NodeList.at(1));
	B.x = it->second.getXcord();B.y = it->second.getYcord();B.z = it->second.getZcord();
	it= this->NodeMap.find(this->NodeList.at(2));
	C.x = it->second.getXcord();C.y = it->second.getYcord();C.z = it->second.getZcord();

	v1.x = A.x - B.x; v1.y = A.y - B.y; v1.z = A.z - B.z;
	v2.x = B.x - C.x; v1.y = B.y - C.y; v1.z = B.z - C.z;

	Norm.x = v1.y*v2.z - v1.z*v2.y;
	Norm.y = -(v2.z*v1.x - v2.x*v1.z);
	Norm.z = v1.x*v2.y - v1.y*v2.x;

	double combsqrt = sqrt(Norm.x*Norm.x+Norm.y*Norm.y+Norm.z*Norm.z);

	this->NormalVector.at(0) = Norm.x/combsqrt;
	this->NormalVector.at(1) = Norm.y/combsqrt;
	this->NormalVector.at(2) = Norm.z/combsqrt;
}
