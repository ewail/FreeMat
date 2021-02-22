function x = test_sparse21
ai = [0,2,6,0,1;3,0,3,0,2;0,0,3,0,2];
ar = [1,2,0,0,4;3,2,0,0,5;0,0,3,0,2];
a = complex(ar+i*ai);
[i,j,v] = find(a);
B = sparse(i,j,v);
x = testeq(a,B);
