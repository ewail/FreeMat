function test_val = test_bitxor1
  test_val = all(bitxor([1,5,42],3) == [2 6 41]);
