
// ###########################################################################################################################
// #                                                                                                                         #
// #  GMESSI :: Translator for The Real ESSI (Real Earthquake-Soil-Structure Interaction) Simulator                          #
// #  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -                                      #
// #                                                                                                                         #
// #  Example_3 : Modelling of Tower above thr ground Surface                                                                #
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


// Size of the soil block in metres
Size = 10;

// Height of the Tower in metres
Height = 6;

// Mesh Size of the soil block 
Mesh_Size = 1;

// Adding Points an1 extruding 
Point(1)={-Size/2,-Size/2,-Size/2};
Extrude{Size,0,0}{Point{1};Layers{Size/Mesh_Size};Recombine;}
Extrude{0,Size,0}{Line{1};Layers{Size/Mesh_Size};Recombine;}
Extrude{0,0,Size}{Surface{5};Layers{Size/Mesh_Size};Recombine;}

// Make the tower located at height 6 m from the ground surface 
Tower = newp;
Point (Tower) = {0,0,Size/2+Height};

//// Create Physical Groups 
Physical Volume ("Soil") = {1};
Physical Surface ("Soil_Base_Surface") ={5};
Physical Surface ("Soil_Top_Surface")  ={27};
Physical Point  ("Tower") = {Tower};

