0
t = linspace(-pi,pi);
subplot(2,2,1)
plot(t,cos(t).*exp(-2*t));
subplot(2,2,2);
plot(t,cos(t*2).*exp(-2*t));
subplot(2,2,3);
plot(t,cos(t*3).*exp(-2*t));
subplot(2,2,4);
plot(t,cos(t*4).*exp(-2*t));
mprint subplot1
