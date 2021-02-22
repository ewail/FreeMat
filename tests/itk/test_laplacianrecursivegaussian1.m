function t = test_laplacianrecursivegaussian1
a = randn(16)*.1; 
a(4:10,4:10) = 5 + randn(7)*.2;
b = laplacianrecursivegaussian(a,1,0);
t = 1;

