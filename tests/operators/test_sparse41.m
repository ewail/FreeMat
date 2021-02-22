function x = test_sparse41
a = [0 0 2 0 3 0
     1 0 2 0 0 3
     1 0 4 5 0 3
     0 0 0 2 0 0];

b = [1 0 0 0 0 0
     0 1 0 0 0 0 
     0 0 1 0 0 0 
       0 0 0 1 0 0];

A = sparse(a);
B = sparse(b);

C = A .* B;
c = a .* b;
x = testeq(c,C);
