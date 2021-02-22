function test_val = test_resize6
  a = zeros(5,500);
  a(6,1) = 32;
  b = zeros(6,500);
  b(6,1) = 32;
  test_val = issame(a,b);
