0
a = 3; b = 7;
x = (0:100)/100; t = x.^(a-1).*(1-x).^(b-1); 
t = t/(sum(t)*.01);
plot(x,t);
mprint betapdf
