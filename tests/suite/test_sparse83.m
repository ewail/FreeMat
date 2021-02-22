function x = test_sparse83
  A = sparse(rand(10));
  B = sparse([]);
  C = [B,B,B;B,B,B,A,B];
  x = testeq(C,A);
