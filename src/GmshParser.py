#!/usr/bin/env python

""" Class for parsing gmsh .msh file

Parses the .msh file and creates List of Nodes,
Elements, Physical Groups, Entity Group ...
"""

__author__ = "Sumeet K. Sinha"
__credits__ = [""]
__license__ = "GPL"
__version__ = "2.0"
__maintainer__ = "Sumeet K. Sinha"
__email__ = "sumeet.kumar507@gmail.com"


from GmshPhysicalGroup import *;
from GmshElement import *;
from GmshNode import *;

class GmshParser:
	'It represents an REAL-ESSI element'

	__mshFile               = None;
	__mshFormat           = None;
	__NodeList            = {};
	__ElementList         = {};
	__PhyGrpList          = {};
	__NewPhyGrpTag        = 1;
	__NewElementTag       = 1;
	__NewNodeTag          = 1;
	__NewEntGrpTag        = 1;

	def __str__(self):
		return "\
\n---------------------------------------\n\
GmshParser -> %s \n---------------------------------------\n\t\
mshFormat::  %s\n\t\
NodeTag::    %s\n\t\
ElementTag:: %s\n\t\
PhysGrpTag:: %s\n\t\
EntGrpTag::  %s\n\t\
" % (self.__mshFile,self.__mshFormat,self.__NewNodeTag,self.__NewElementTag,self.__NewPhyGrpTag,\
	self.__NewEntGrpTag)

	def __init__ (self,mshFile):
		""" Define GmshParser class with output file

		Class Variables:
		mshFile        -- GMSH .msh file
		"""

		self.__mshFile       = mshFile;
		self.__parse();

	def __parse (self):
		""" parse the gmsh .msh file """

		try:

			Lines  = [];
			phyGrp = None;

			with open(self.__mshFile) as file:
				Lines =file.read().splitlines();
			
			for i in range(0,len(Lines)):
				line = Lines[i];
				if(line=="$MeshFormat"):
					i=i+1;
					self.__mshFormat=Lines[i];
					i=i+1;
				elif(line=="$PhysicalNames"):
					i=i+1;
					NumPhyGrp = int(Lines[i]);
					i=i+1;
					for j in range(0,NumPhyGrp):
						PhyGroup = GmshPhysicalGroup(Lines[i+j]);
						self.__PhyGrpList[PhyGroup.Tag()]=PhyGroup;
						if(PhyGroup.Tag()>self.__NewPhyGrpTag):
							self.__NewPhyGrpTag = PhyGroup.Tag();
					i=i+1;
				elif(line=="$Nodes"):
					i=i+1;
					NumNodes = int(Lines[i]);
					i=i+1;
					for j in range(0,NumNodes):
						Node = GmshNode(Lines[i+j]);
						self.__NodeList[Node.GmshTag()]=Node;
						if(Node.GmshTag()>self.__NewNodeTag):
							self.__NewNodeTag = Node.GmshTag();
					i=i+1;
				elif(line=="$Elements"):
					i=i+1;
					NumElements = int(Lines[i]);
					i=i+1;
					for j in range(0,NumElements):
						Element = GmshElement(Lines[i+j]);
						self.__ElementList[Element.GmshTag()]=Element;
						if(Element.GmshTag()>self.__NewElementTag):
							self.__NewElementTag = Element.GmshTag();
					i=i+1;

		except ValueError:
			print("GmshParser:: Gmsh File could not be parsed\
				Try again...")

		## Fill the nodelist and elementlist of physical group
		for x in self.__ElementList:
			element = self.__ElementList[x];
			phyTag  = element.PhyGrpTag();
			connectivity  = element.Connectivity();

			phyGrp = self.__PhyGrpList[phyTag] ;

			phyGrp.addNode(connectivity);
			phyGrp.addElement([x]);


	def addNode(self,Node):
		""" add node tag to NodeList """
		self.__NodeList[Node.GmshTag()]=Node;
		if(Node.GmshTag()>=self.__NewNodeTag):
			self.__NewNodeTag = Node.GmshTag() +1;

	def addElement(self,Element):
		""" add element tag to ElementList """
		self.__ElementList[Element.GmshTag()]=Element;
		if(Element.GmshTag()>=self.__NewElementTag):
			self.__NewElementTag = Element.GmshTag() +1;

	def addPhyGrp(self,PhysicalGroup):
		""" add element tag to ElementList """
		self.__PhyGrpList[PhysicalGroup.Tag()]=PhysicalGroup;
		if(PhysicalGroup.Tag()>=self.__NewPhyGrpTag):
			self.__NewPhyGrpTag = PhysicalGroup.Tag() +1;

	def NodeList(self):
		""" return NodeList """
		return self.__NodeList;

	def ElementList(self):
		""" return ElementList """
		return self.__ElementList;
	
	def PhyGrpList(self):
		""" return PhysGrpList """
		return self.__PhyGrpList;		

	def newNodeTag(self):
		""" return new gmsh Node Tag """
		return self.__newNodeTag;

	def newElementTag(self):
		""" return new gmsh Element Tag """
		return self.__newElementTag;		

	def newPhyGrpTag(self):
		""" return new gmsh PhysGrp Tag """
		return self.__NewPhyGrpTag;

	def newEntGrpTag(self):
		""" return new gmsh Ent Tag """
		return self.__NewEntGrpTag;


GmshParserRep = "Example_1.msh";
X = GmshParser(GmshParserRep);

print X

PhyGrpList = X.PhyGrpList();
for t in PhyGrpList:
	print PhyGrpList[t];


