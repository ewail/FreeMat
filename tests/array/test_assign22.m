function test_val = test_assign22
  A = [1,2,3];
  test_val = 0;
  try
    A(0) = 2;
  catch
    test_val = 1;
  end
