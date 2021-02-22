function A = jit_test010
  B = 1:100000;
  A = zeros(1,100000);
  for i=1:100000
    A(i) = B(i);
  end
