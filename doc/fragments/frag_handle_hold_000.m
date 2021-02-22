0
x = linspace(-5,5,500);
t = exp(-x.^2);
y = t.*cos(2*pi*x*3);
plot(x,y);
mprint hold1
