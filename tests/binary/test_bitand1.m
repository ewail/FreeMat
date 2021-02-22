function test_val = test_bitand1
 test_val = all(bitand([1,5,42],3) == [1 1 2]);
