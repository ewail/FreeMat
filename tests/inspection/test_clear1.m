function test_val = test_clear1
  A = 1;
  clear('A');
  test_val = 0;
  try
    B = A;
  catch
    test_val = 1;
  end;
