function test_val = test_resize3
  a = 0;
  b = zeros(20);
  for i=1:20;
    a(i,i) = i;
    b(i,i) = i;
  end
  test_val = issame(a,b);
