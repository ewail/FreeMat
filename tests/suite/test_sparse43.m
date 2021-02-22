function x = test_sparse43
a = [0 0 2 0 3 0
     1 0 2 0 0 3
     1 0 4 5 0 3
     0 0 0 2 0 0];

b = pi;

A = sparse(a);

C = A .* b;
c = a .* b;
x = testeq(c,C);
