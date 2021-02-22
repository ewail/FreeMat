function x = test_sparse20
a = [0,0,3,1,3;1,0,0,0,2;4,3,0,2,0];
[i,j,v] = find(a);
B = sparse(i,j,v);
x = testeq(a,B);
