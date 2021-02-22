function x = test_sparse16
a = float([1,2,0,0,4;3,2,0,0,5;0,0,3,0,2]);
A = sparse(a);
c = a(2,5);
C = A(2,5);
x = testeq(c,C);
