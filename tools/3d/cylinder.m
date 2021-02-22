function y = cylinder
  p0_top = [0,0,.5];
  p0_bottom = [0,0,-.5];
  y = [];
  del = 5;
  for i=1:72
    x_this = cosd((i-1)*del);
    y_this = sind((i-1)*del);
    x_next = cosd(i*del);
    y_next = sind(i*del);
    p1 = [x_this,y_this,.5];
    p2 = [x_this,y_this,-.5];
    p3 = [x_next,y_next,.5];
    p4 = [x_next,y_next,-.5];
    y = [y;p1;p0_top;p3;p4;p0_bottom;p2];
    y = [y;quad(p1,p2,p4,p3)];
  end
  