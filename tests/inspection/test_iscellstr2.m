function test_val = test_iscellstr2
  A = {'Hello',2};
  test_val = ~iscellstr(A);
