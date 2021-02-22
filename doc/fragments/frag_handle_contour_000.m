0
[x,y] = meshgrid(linspace(-1,1,25),linspace(-2,2,30));
z = x.*exp(-x.^2-y.^2);
contour(z)
mprint contour1
