0
t=0:(2*pi/100):(2*pi);
x=cos(t*2).*(2+sin(t*3)*.3);
y=sin(t*2).*(2+sin(t*3)*.3);
z=cos(t*3)*.3;
subplot(2,2,1)
plot(t,x);
subplot(2,2,2);
plot(t,y);
subplot(2,2,3);
plot(t,z);
subplot(2,2,4);
tubeplot(x,y,z,0.14*sin(t*5)+.29,t,10)
axis equal
view(3)
mprint subplot3
