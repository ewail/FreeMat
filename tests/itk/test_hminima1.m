function t = test_hminima1
  a = zeros(5);
  a(2:4,2:4) = 5; a(3,3) = 2;
  b = hminima(a,3);
  c = 3*ones(5); c(2:4,2:4) = 5;
  t = issame(b,c);
