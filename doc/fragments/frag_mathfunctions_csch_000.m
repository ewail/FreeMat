0
x1 = -pi+.01:.01:-.01;
x2 = .01:.01:pi-.01;
plot(x1,csch(x1),x2,csch(x2)); grid('on');
mprint('cschplot');
