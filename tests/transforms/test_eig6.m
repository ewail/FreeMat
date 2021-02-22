function t = test_eig6
  try
    eig([1,2;3,4],eye(1));
  catch
  end
  t = true;
