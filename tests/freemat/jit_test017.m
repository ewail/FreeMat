function A = jit_test017
  A = 0;
  for j=1:100
    B = j;
    for i=1:10000
      A = A + B;
    end
  end
