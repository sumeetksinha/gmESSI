Point (1) = {0,0,0};
 Extrude (4,0,0) {Point{1}; Layers{5};}
 Extrude (0,1,0) {Line{1}; Layers{4};Recombine;}
 Extrude (0,0,1) {Surface{5}; Layers{{1,2},{0.6,1}};Recombine;}
//Physical Volume ("$Brick$ <AddAllNode{m,3}> <27NodeBrick{1}> ") ={1};
//Physical Surface ("$FixedSurface$ <Fix{all}>")={26};
//Physical Surface("$UpperFace$") ={22};
//Physical Point ("All Points") ={1,2,3,4,5,6,10,14};
//Physical Surface("All Surface group") = {5,14,22,27,18,26};
//Physical Line("All Lines") ={1,2,3,4,12,13,21,17,7,8,9,10};
//Physical Volume("All Volume") ={1};

