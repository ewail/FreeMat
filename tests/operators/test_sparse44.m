function x = test_sparse44
ar = [0 0 2 0 3 0
      1 0 2 0 0 3
      1 0 4 5 0 3
      0 0 0 2 0 0];

ai = [1 0 1 2 0 0
      1 0 0 3 0 0
      0 0 2 0 0 4
      0 0 0 3 0 0];

a = ar + i * ai;
b = 3 + 4*i;

A = sparse(a);

C = A .* b;
c = a .* b;
x = testeq(c,C);
