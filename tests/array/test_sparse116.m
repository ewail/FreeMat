function x = test_sparse116
  a = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2];
  A = sparse(a);
  p = [3;4;5;9;10;20];
  a(p) = 1;
  A(p) = 1;
  x = testeq(a,A);
