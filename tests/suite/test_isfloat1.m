function test_val = test_isfloat1
  test_val = isfloat(3) && ~isfloat('hello');
