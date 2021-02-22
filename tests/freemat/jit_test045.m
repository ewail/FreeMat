function A = jit_test045
  A = zeros(1,1000000);
  for i = 1:1000000
    A(i) = (i + 3 - 4 + A(i))*2;
  end
