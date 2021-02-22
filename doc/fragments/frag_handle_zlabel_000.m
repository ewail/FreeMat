0
t = linspace(0,5*pi);
x = cos(t);
y = sin(t);
z = t;
plot3(x,y,z,'r-');
view(3);
zlabel('time');
mprint zlabel1
