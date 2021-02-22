function test_val = test_isinteger1
  test_val = ~isinteger(single(3)) && isinteger(uint8(3));
