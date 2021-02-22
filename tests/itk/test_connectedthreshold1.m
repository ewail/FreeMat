function test_val = test_connectedthreshold1
  a = zeros(7);
  a(2:3,2:6) = 1;
  b = connectedthreshold(a,[2;2],.5,2,6);
  c = zeros(7);
  c(2:3,2:6) = 6;
  test_val = issame(c,b);
