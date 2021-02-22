function test_val = test_getfield1
  x = rand(10);
  a.foo = x;
  b = getfield(a,'foo');
  test_val = issame(x,b);
