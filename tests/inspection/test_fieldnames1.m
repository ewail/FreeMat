function test_val = test_fieldnames1
  x.foo = 3; x.goo = 'hello';
  y = fieldnames(x);
  z = {'foo';'goo'};
  test_val = issame(y,z);
