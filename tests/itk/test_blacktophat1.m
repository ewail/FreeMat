function test_val = test_blacktophat1
  a = ones(5);
  a(3,3) = 0;
  b = blacktophat(a);
  c = zeros(5);
  c(3,3) = 1;
  test_val = issame(b,c);
