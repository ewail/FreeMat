function test_val = test_isset1
  a = []; b = 1;
  test_val = ~isset('c') && ~isset('a') && isset('b');
