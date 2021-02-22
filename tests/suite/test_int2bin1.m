function test_val = test_int2bin1
  test_val = all(int2bin([4;3;2;1],3)==[1,0,0;0,1,1;0,1,0;0,0,1]);
