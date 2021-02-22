function t = test_hconvex
  a = zeros(5);
  a(2:4,2:4) = 1; a(3,3) = 5;
  b = hconvex(a,3);
  c = zeros(5);
  c(3,3) = 3;
  t = issame(b,c);
