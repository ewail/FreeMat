function A = jit_test009
  B = ones(1,10000);
  A = 0;
  for k=1:10000
    A = A + B(k);
  end
