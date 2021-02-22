function test_val = test_lasterr1
  try
    error('Hulabaloo');
  catch;
  end;
  test_val = strcmp(lasterr,'Hulabaloo');
