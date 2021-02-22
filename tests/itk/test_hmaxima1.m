function t = test_hmaxima1
  a = zeros(5);
  a(2:4,2:4) = 5; a(3,3) = 2;
  b = hmaxima(a,3);
  c = zeros(5);  c(2:4,2:4) = 2;
  t = issame(b,c);
