function t = test_laplaciansharpening1
a = zeros(5);
a(2:4,2:4) = 0.5;
a(3,3) = 1;
b = laplaciansharpening(a);
c = zeros(5);
c(2:4,2) = [.9;.5;.9];
c(2:4,4) = [.9;.5;.9];
c(2:4,3) = [.5;1;.5];
t = norm(c-b,'fro') < 0.01;

