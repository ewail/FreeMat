0
p = polyfit(x,y,11)
f = polyval(p,x);
plot(x,y,'r-',x,f,'ko');
mprint polyfit3
