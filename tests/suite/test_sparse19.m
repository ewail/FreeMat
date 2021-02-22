function x = test_sparse19
ai = [0,2,6,0,1;3,0,3,0,2;0,0,3,0,2];
ar = [1,2,0,0,4;3,2,0,0,5;0,0,3,0,2];
a = dcomplex(ar+i*ai);
A = sparse(a);
c = a(2,5);
C = A(2,5);
x = testeq(c,C);
