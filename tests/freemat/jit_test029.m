function A = jit_test029
  A = zeros(10000,1);
  for i=1:10000
    A(i,1) = 5*(i - 4);
  end
