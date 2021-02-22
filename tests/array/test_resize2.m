function test_val = test_resize2
  a = 0;
  b = zeros(100,1);
  for i=1:100
    a(i,1) = i;
    b(i,1) = i;
  end
  test_val = issame(a,b);

