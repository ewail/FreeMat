function t = test_curvatureanisotropicdiffusionfilter1
a = randn(16)*.1, a(4:10,4:10) = 5 + randn(7)*.2;
z = zeros(128);
z(65:80,65:80) = a;
b = curvatureanisotropicdiffusionfilter(z,30,1/30,.1);
t = 1;
