0
[x,y] = meshgrid([-2:.25:2]);
z=x.*exp(-x.^2-y.^2);
contour3(x,y,z,30);
axis square;
view(-15,25)
mprint contour3_1
