0
x = linspace(-1,1,30);
y = legendre(4,x);
plot(x,y); xlabel('x'); ylabel('legendre(4,x)');
mprint legendre
