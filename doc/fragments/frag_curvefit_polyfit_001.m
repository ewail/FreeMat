0
p = polyfit(x,y,3)
f = polyval(p,x);
plot(x,y,'r-',x,f,'ko');
mprint polyfit2
