0
x = linspace(-pi,pi);
sx = sin(x); cx = cos(x);
plot(x,atan(sx./cx),x,atan2(sx,cx))
mprint('atan2plot');
