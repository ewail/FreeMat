function test_val = test_whitetophat1
  a = zeros(5);
  a(3,3) = 1;
  b = whitetophat(a);
  test_val = issame(b,a);
