function t = test_hconcave1
  a = zeros(5);
  a(2:4,2:4) = 5; a(3,3) = 2;
  b = hconcave(a,3);
  c = 3*ones(5); c(2,2:4) = 0;
  c(4,2:4) = 0; c(2:4,2) = 0; c(2:4,4) = 0;
  t = issame(b,c);
