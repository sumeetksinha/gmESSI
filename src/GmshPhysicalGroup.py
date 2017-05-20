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

class GmshPhysicalGroup:
	'It represents an gmsh physical group'

	__Tag 		   = -1;
	__Id  		   = None;
	__Type		   = -1;
	__NodeSet      = None;
	__ElementSet   = None;

	def __str__(self):
		return "\
\n---------------------------------------\n\
GmshPhysicalGroup -> %s\n---------------------------------------\n\t\
Type::        %s\n\t\
Tag::         %s\n\t\
ElementList:: %s\n\t\
NodeList::    %s\n\t\
" % (self.__Id,self.__Type,self.__Tag,self.__ElementSet,self.__NodeSet)

	def __init__ (self,Type,Tag,Id=None):
		""" Define GmshPhysicalGroup class with output file

		Class Variables:
		Tag            -- GMSH tag for the physical group
		Id             -- String representation of physical group tag
		Type           -- Gmsh element order type 
		"""

		self.__Tag       = GmshTag;
		self.__Id        = Connectivity;
		self.__Type      = PhysicalTag;

	def __init__ (self,GmshPhysicalGroupRep):

		try:

			Tokens = GmshPhysicalGroupRep.split();
			# print Tokens

			self.__Type   = int(Tokens[0]);
			self.__Tag    = int(Tokens[1]);
			if(len(Tokens)==3):
				self.__Id = (Tokens[2])[1:-1];

		except ValueError:
			print("GmshPhysicalGroup:: Gmsh File has invalid symbols in Physical Group Section\
				Try again...")

	def addNode(self,Node):
		""" add node tag to NodeList """
		if(self.__NodeSet==None):
			self.__NodeSet = set();
		self.__NodeSet.update(Node);
		# self.__NodeSet = set(self.__NodeSet);

	def addElement(self,Element):
		""" add element tag to ElementList """
		if(self.__ElementSet==None):
			self.__ElementSet = set();
		self.__ElementSet.update(Element);
		# self.__ElementSet = set(self.__ElementSet);

	def NodeSet(self):
		""" return NodeList """
		return self.__NodeSet;

	def ElementSet(self):
		""" return ElementList """
		return self.__ElementSet;

	def Tag(self):
		""" return Tag """
		return self.__Tag;
		
	def Id(self):
		""" return Id """
		return self.__Id;
		
	def Type(self):
		""" return Type """
		return self.__Type;
		

# GmshPhysicalGroupRep = """2 2 "Base_Surface" """;
# X = GmshPhysicalGroup(GmshPhysicalGroupRep);

# print X
