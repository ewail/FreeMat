function x = test_sparse94
A = float(int32(10*sprandn(10,10,0.3)));
B = float(int32(10*sprandn(10,10,0.3)));
C = A ~= B;
c = full(A) ~= full(B);
x = testeq(c,C);
C = A ~= -2;
c = full(A) ~= -2;
x = x & testeq(c,C);
C = 2 ~= A;
c = 2 ~= full(A);
x = x & testeq(c,C);
