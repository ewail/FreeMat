function x = test_sparse81
A = sparse(rand(10));
C = [];
D = [C,A];
x = testeq(D,A);
