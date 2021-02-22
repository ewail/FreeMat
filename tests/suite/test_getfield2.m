function test_val = test_getfield2
  x = rand(10);
  a(2,3).foo = x;
  b = getfield(a, {2,3}, 'foo');
  test_val = issame(x,b);
