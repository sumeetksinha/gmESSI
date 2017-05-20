#!/usr/bin/env python

""" Class for gmsh elements

It containts the information about GMSH Element
"""

__author__ = "Sumeet K. Sinha"
__credits__ = [""]
__license__ = "GPL"
__version__ = "2.0"
__maintainer__ = "Sumeet K. Sinha"
__email__ = "sumeet.kumar507@gmail.com"

from GmshElementType import *;

class GmshElement:
	'It represents an REAL-ESSI element'

	__GmshTag               = -1;
	__EssiTag               = -1;
	__NumNodes              = -1;
	__Connectivity          = -1;
	__PhyGrplId            = None;
	__PhyGrpTag             = -1;
	__EntGrpTag             = -1;
	__Type                  = -1;
	__Name                  = "GmshElement";

	def __str__(self):
		return "\
\n---------------------------------------\n\
GmshElement -> %s \n---------------------------------------\n\t\
GmshTag::      %s\n\t\
EssiTag::      %s\n\t\
NumNodes::     %s\n\t\
Connectivity:: %s\n\t\
PhyGrpId::     %s\n\t\
PhyGrpTag::    %s\n\t\
EntGrpTag::    %s\n\t\
" % (self.__Name, self.__GmshTag,self.__EssiTag,self.__NumNodes,self.__Connectivity,\
	self.__PhyGrplId,self.__PhyGrpTag,self.__EntGrpTag)

	def __init__ (self,GmshTag,Connectivity,PhyGrpTag=-1,EntGrpTag=-1,EssiTag=-1,Type=None):
		""" Define GmshElement class with output file

		Class Variables:
		GmshTag        -- GMSH tag for the element
		Connectivity   -- Connectivity in gmsh
		PhyGrpTag    -- Physical tag in Gmsh 
		EntGrpTag      -- Entity tag in Gmsh 
		EssiTag        -- Element tag in REAL ESSI
		NumNodes       -- Number of Nodes in Element
		Type           -- Gmsh element type 
		"""

		self.__GmshTag       = GmshTag;
		self.__Connectivity  = Connectivity;
		self.__PhyGrpTag   = PhyGrpTag;
		self.__EntGrpTag     = EntGrpTag;
		self.__EssiTag       = EssiTag;
		self.__NumNodes      = len(Connectivity);
		self.__Type          = Type;
		self.__Name          = GmshElementType[Type];

	def __init__ (self,GmshElementRep):

		try:

			Tokens = GmshElementRep.split();
			Tokens = [int(i) for i in Tokens]

			self.__GmshTag = Tokens[0];
			self.__Type    = Tokens[1];
			nofTag       = Tokens[2];
			self.__PhyGrpTag = Tokens[3];
			self.__EntGrpTag   = Tokens[4];
			self.__Connectivity= Tokens[4+nofTag-2:];
			self.__NumNodes    = len(self.__Connectivity);
			self.__Name        = GmshElementType[self.__Type];

		except ValueError:
			print("GmshElement:: Gmsh File has invalid symbols in Element Section\
				Try again...")

	def GmshTag(self):
		""" return gmsh element tag """
		return self.__GmshTag;

	def GmshTag(self):
		""" return gmsh element tag """
		return self.__GmshTag;

	def EssiTag(self):
		""" return element EssiTag """
		return self.__EssiTag;

	def NumNodes(self):
		""" return element NumNodes """
		return self.__NumNodes;

	def Connectivity(self):
		""" return element Connectivity """
		return self.__Connectivity;

	def PhysicalId(self):
		""" return element PhysicalId """
		return self.__PhyGrplId;

	def PhyGrpTag(self):
		""" return element PhyGrpTag """
		return self.__PhyGrpTag;

	def EntGrpTag(self):
		""" return element EntGrpTag """
		return self.__EntGrpTag;

	def Type(self):
		""" return element Type """
		return self.__Type;

	def Name(self):
		""" return element Name """
		return self.__Name;

# GmshElementRep = "33 5 2 1 1 58 62 64 60 53 55 56 54";
# X = GmshElement(GmshElementRep);

# print X
