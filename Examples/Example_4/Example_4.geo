
// ###########################################################################################################################
// #                                                                                                                         #
// #  GMESSI :: Translator for The Real ESSI (Real Earthquake-Soil-Structure Interaction) Simulator                          #
// #  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -                                      #
// #                                                                                                                         #
// #  Example_1 : Modelling of a concrete foundation on Soil connected by truss elements                                     #
// #              (perfect contact) foundation and soil is always connected                                                  #
// #                                                                                                                         #
// #  GITHUB:: https://github.com/SumeetSinha/gmESSI.git                                                                     #
// #                                                                                                                         #
// #  *** gmsh geometry file  for this example ***                                                                           #
// #                                                                                                                         #
// #  Sumeet Kumar Sinha (September,2016)                                                                                    #
// #  Computational Geomechanics Group                                                                                       #
// #  University of California, Davis                                                                                        #
// #  s u m e e t k s i n h a . c o m                                                                                        #
// ########################################################################################################################### 


// Size of the soil block 
Size = 1;

// Thickness of Foundation
Thick = 0.1;
Foundation_Layers = 2;

//// Mesh Size of the block 
Mesh_Size = 0.2;

// Adding Points an1 extruding 
Point(1)={-Size/2,-Size/2,-Size/2};
Extrude{Size,0,0}{Point{1};Layers{Size/Mesh_Size};Recombine;}
Extrude{0,Size,0}{Line{1};Layers{Size/Mesh_Size};Recombine;}
Extrude{0,0,Size}{Surface{5};Layers{Size/Mesh_Size};Recombine;}

// Make sure in Tools -> Geometry -> General 
// Geometry tolerence is set smaller than Epsilon
// such as Geometry tolerence = 1e-14

Epsilon = 1e-8;
Translate {0, 0, Epsilon} {Duplicata{Surface{27};}}
Transfinite Line {29,30,31,32} = Size/Mesh_Size +1;
Transfinite Surface {28};
Recombine Surface {28};

//// Extruding the surface to foundation thickness
Extrude{0,0,Thick}{Surface{28};Layers{Foundation_Layers};Recombine;}

//// Create Physical Groups 
Physical Volume ("Soil") = {1};
Physical Surface ("Soil_Base_Surface") ={5};
Physical Surface ("Soil_Top_Surface")  ={27};
Physical Surface ("Foundation_Base_Surface")={28};
Physical Surface ("Foundation_Top_Surface") ={54};
Physical Volume  ("Foundation") = {2};


Physical Surface("Fix_X") = {26, 53, 45, 18};
Physical Surface("Fix_Y") = {22, 49, 14, 41};
Physical Surface("6_Dofs") = { 27,28};
Physical Volume("3_Dofs") = {1,2};
