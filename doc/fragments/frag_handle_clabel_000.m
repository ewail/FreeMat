0
[x,y] = meshgrid(linspace(-1,1,50));
z = x.*exp(-(x.^2+y.^2));
h = contour(z);
clabel(h,'backgroundcolor',[1,1,.6],'edgecolor',[.7,.7,.7]);
mprint clabel1
