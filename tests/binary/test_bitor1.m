function test_val = test_bitor1
  test_val = all(bitor([1,5,42],3) == [3 7 43]);
