function x = test_sparse27
a = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2];
A = sparse(a);
p = [3;4;5;9;10];
a(p) = 7;
A(p) = 7;
x = testeq(a,A);
