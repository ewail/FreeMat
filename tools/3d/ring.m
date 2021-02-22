function y = ring(frac)
del = 5;
steps = 360/del;
y = [];
for i=1:steps
  p1 = [frac*cosd((i-1)*del),frac*sind((i-1)*del),0.5];
  p2 = [frac*cosd(i*del),frac*sind(i*del),0.5];
  p3 = [frac*cosd((i-1)*del),frac*sind((i-1)*del),-0.5];
  p4 = [frac*cosd(i*del),frac*sind(i*del),-0.5];
  p5 = [cosd((i-1)*del),sind((i-1)*del),0.5];
  p6 = [cosd(i*del),sind(i*del),0.5];
  p7 = [cosd((i-1)*del),sind((i-1)*del),-0.5];
  p8 = [cosd(i*del),sind(i*del),-0.5];
  y = [y;
       quad(p2,p1,p5,p6);
       quad(p8,p6,p5,p7);
       quad(p4,p3,p1,p2);
       quad(p7,p3,p4,p8);];
end
  