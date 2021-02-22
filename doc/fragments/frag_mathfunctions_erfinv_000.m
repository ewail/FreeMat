0
x = linspace(-.9,.9,100);
y = erfinv(x);
plot(x,y); xlabel('x'); ylabel('erfinv(x)');
mprint erfinv1
