function A = jit_test008
  A = zeros(1,10000);
  for i=1:10010;
    A(i) = i;
  end
