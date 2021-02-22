function test_val = test_resize7
  a = zeros(5,5);
  a(5,500) = 32;
  b = zeros(5,500);
  b(5,500) = 32;
  test_val = issame(a,b);
