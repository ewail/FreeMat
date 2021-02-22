function A = jit_test025
  B = [3,4,5];
  C = [1;2;3];
  A = zeros(1,10000);
  for i=1:10000
    A(i) = B*C;
  end
