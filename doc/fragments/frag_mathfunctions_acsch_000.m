0
x1 = -20:.01:-1;
x2 = 1:.01:20;
plot(x1,acsch(x1),x2,acsch(x2)); grid('on');
mprint('acschplot');
