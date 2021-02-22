0
x = repmat(linspace(-1,1),[100,1]);
y = x';
r = x.^2+y.^2;
z = exp(-r*3).*cos(5*r);
c = r;
surf(x,y,z,c)
axis equal
view(3)
mprint surf1
