function A = jit_test021
  A = zeros(100,100);
  for i=1:(100*100)
    A(i) = i;
  end
