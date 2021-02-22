function t = test_noisefilter1
a = randn(16)*.1; 
a(4:10,4:10) = 5 + randn(7)*.2;
b = noisefilter(a,1);
t = 1;

