/****************************************************************************
* 								SurfaceNormal.h
* 								===============
*
* AUTHOR: Sumeet Kumar Sinha (sumeet.kumar507@gmail.com)
* CREATED: 9-07-2015
*******************************************************************************/

#ifndef SURFACENORMAL_H
#define SURFACENORMAL_H

#include <vector>
#include <math.h>
#include <map>
#include "Node.h"

struct Vector{

	double x,y,z;
};

class SurfaceNormal{

	public:

		SurfaceNormal();
		SurfaceNormal(const vector<int> nodeList, const map<int,Node> nodeMap);
		~SurfaceNormal();

		void setNodeList(const vector<int> nodeList);
		void setNodeMap(const map<int,Node> nodeMap);
		vector<double> getNormalVector();

	private:

		void formNormalVector();
		vector<int> NodeList;
		map<int,Node> NodeMap;
		vector<double> NormalVector;
};

#endif //SURFACENORMAL_H