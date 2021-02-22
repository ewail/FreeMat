function A = jit_test011
  A = zeros(1,100000);
  B = 5;
  for i=1:100000
    C = B + i;
    A(i) = C;
  end
