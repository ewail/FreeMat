function test_val = test_clear2
  A = 1;
  clear('all');
  test_val = 0;
  try
    B = A;
  catch
    test_val = 1;
  end;
