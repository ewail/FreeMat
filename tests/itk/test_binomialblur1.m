function test_val = test_binomialblur1
  a = zeros(5);
  a(3,3) = 1;
  b = binomialblur(a,1);
  c = [0,1/4,1/2,1/4,0]; d = c'*c;
  test_val = issame(d,b);
