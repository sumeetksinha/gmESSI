  Point(1)= {0,0,0}; 
  Extrude{1,0,0} {Point{1};Layers{10};Recombine;}
  Extrude{0,1,0} {Line{1};Layers{10};Recombine;}
  Extrude{0,0,1} {Surface{5};Layers{10};Recombine;}

  Physical Surface ("$Surface1$ ") ={5};
  // Generating 1 layers of elements between Physical group 1 and 2
  Physical Surface ("$Surface2$ [Connect{Physical_Group#1,Physical_Group#2,Physical_Group#3,{0\0\1},1,1,0,0.005, My_New_Physical_Group}]") ={27};
  // Generating 5 layers of elements between Physical group 1 and 2
  // Physical Surface ("$Surface2$ [Connect{Physical_Group#1, Physical_Group#2, Physical_Group#3, {0\0\1}, 0.2, 5, 1, 0.005, My_New_Physical_Group}]") ={27};
  Physical Volume  ("$Volume$") ={1};
  