function A = jit_test052
  A = 0;
  B = 1;
  GOOGLEPLEX = [4323,52342];
  for i=1:10000
    A = func_jit_test052(A,B);
  end
