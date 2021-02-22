function x = test_subset20
  a = [];
  b = rand(14,1);
  a(:,1) = b;
  x = testeq(a,b);
