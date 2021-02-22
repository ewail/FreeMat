0
t = linspace(0,2*pi,128);
x = cos(15*t);
y = fft(x);
plot(t,abs(y));
mprint('fft1');
