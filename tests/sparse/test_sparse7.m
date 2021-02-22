function x = test_sparse7
a = [0,0,2,0,0;0,1,0,1,0;0,0,0,1,3];
A = sparse(a);
b = [1,4,5;2,5,15;3,6,11];
C = A(b);
c = a(b);
x = testeq(c,C);
