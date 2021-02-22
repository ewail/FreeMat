function test_val = test_error1
  test_val = false;
  try
     error('test')
  catch
     test_val = true;
  end
