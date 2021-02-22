function test_val = test_resize1
  a = 0;
  b = zeros(1,100);
  for i=1:100
    a(i) = i;
    b(i) = i;
  end
  test_val = issame(a,b);
