function t = test_bsplineupsample1
  a = zeros(3);
  a(2,2) = 1;
  b = bsplineupsample(a);
  c = zeros(6);
  c(3:4,3:4) = 1;
  t = issame(c,b);