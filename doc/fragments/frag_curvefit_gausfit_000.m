0
t = linspace(-pi,pi); 
y = cos(t);
[mu,sigma,dc,gain,yhat] = gausfit(t,y);
plot(t,y,'rx',t,yhat,'g-');
mprint gausfit1
