function test_val = test_thresholdlabel1
  a = zeros(7);
  a(2:3) = 1;
  a(5:6) = 2;
  b = thresholdlabel(a,.5);
  test_val = issame(b,double(a>0));
