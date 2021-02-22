function t = test_median1
  a = zeros(5);
  a(2:4,2:4) = 1; a(3,3) = 5;
  b = medianfilter(a,[1,1]);
  c = zeros(5);
  c(3,2:4) = 1;
  c(2:4,3) = 1;
  t = issame(b,c);
