function A = jit_test047
  A = zeros(1,1000000);
  p = 1:10;
  for i=1:90000
    A(i) = p(5);
  end
