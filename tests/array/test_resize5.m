function test_val = test_resize4
  a = 0;
  b = zeros(20);
  for j=1:20
    for i=1:20
     a(i,j) = i-j;
     b(i,j) = i-j;
   end
  end
  test_val = issame(a,b);
