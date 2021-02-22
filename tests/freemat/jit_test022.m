function A = jit_test022
  A = 1:1000000;
  B = 0;
  for i=1:1000000;
    B = B + A(i);
  end
  A = B;
