0
x1 = -20:.01:-1;
x2 = 1:.01:20;
plot(x1,imag(asech(x1)),x2,imag(asech(x2))); grid('on');
mprint('asechplot');
