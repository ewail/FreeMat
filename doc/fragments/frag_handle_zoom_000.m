0
x = linspace(-1,1,300)'*ones(1,600);
y = ones(300,1)*linspace(-1,1,600);
Z = exp(-(x.^2+y.^2)/0.3);
image(Z);
zoom(1.0);
mprint zoom1
