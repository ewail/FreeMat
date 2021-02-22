function test_val = test_dilate1
  a = zeros(5);
  a(3,3) = 1;
  b = dilate(a);
  c = zeros(5,'logical');
  c(2:4,2:4) = 1;
  test_val = issame(c,b);