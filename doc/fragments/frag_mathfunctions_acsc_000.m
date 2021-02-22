0
x1 = -10:.01:-1.01;
x2 = 1.01:.01:10;
plot(x1,acsc(x1),x2,acsc(x2)); grid('on');
mprint('acscplot');
