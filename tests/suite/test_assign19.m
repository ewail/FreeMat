function test_val = test_assign19
  g(3).foo = 3;
  g(1) = [];
  test_val = (g(2).foo == 3);
