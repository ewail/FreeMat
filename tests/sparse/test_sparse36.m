function x = test_sparse36
a = rand(8) + i*rand(8);
A = sparse(a);
B = float(A);
b = full(B);
x = testeq(b,B);
