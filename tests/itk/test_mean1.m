function t = test_mean1
  a = zeros(5);
  a(3,3) = 1;
  b = meanfilter(a,[1,1]);
  c = zeros(5);
  c(2:4,2:4) = 1/9;
  t = issame(b,c);
