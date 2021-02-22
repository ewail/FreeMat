function x = test_sparse54
a = rand(200,100);
b = rand(100,300);
b(b>0.1) = 0;
c = a*b;
B = sparse(b);
C = a*B;
x = testeq(c,C);
