function y = cube(x0,y0,z0,halfx,halfy,halfz)
  % Define the 8 points of the cube
  p1 = [x0-halfx,y0-halfy,z0-halfz];
  p2 = [x0+halfx,y0-halfy,z0-halfz];
  p3 = [x0-halfx,y0+halfy,z0-halfz];
  p4 = [x0+halfx,y0+halfy,z0-halfz];
  p5 = [x0-halfx,y0-halfy,z0+halfz];
  p6 = [x0+halfx,y0-halfy,z0+halfz];
  p7 = [x0-halfx,y0+halfy,z0+halfz];
  p8 = [x0+halfx,y0+halfy,z0+halfz];
  y = [quad(p6,p2,p4,p8);
       quad(p5,p6,p8,p7);
       quad(p1,p3,p4,p2);
       quad(p1,p5,p7,p3);
       quad(p3,p7,p8,p4);
       quad(p5,p1,p2,p6)];
       
       
       