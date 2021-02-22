function x = test_sparse34
a = sparse([0;0;2;3;0;0;4;5;0;0;0;2]);
n = [7,9,1,3,2,3,1,2,2];
b = full(a);
a(n) = [];
b(n) = [];
x = testeq(a,b);
