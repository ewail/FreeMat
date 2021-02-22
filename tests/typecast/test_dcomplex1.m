function test_val = test_dcomplex1
  A = dcomplex(2+3i);
  B = double(2+3i);
  test_val = issame(A,B);
