0
t = linspace(0,1,256);
A = [exp(-(t-1.0).^2/0.1);exp(-(t-0.5).^2/0.1);exp(-t.^2/0.1)]';
plot(A);
mprint('colormap4');
