0
x = linspace(-1,1,512)'*ones(1,512);
y = x';
Z = exp(-(x.^2+y.^2)/0.3);
image(Z);
colormap(copper);
mprint copper1
