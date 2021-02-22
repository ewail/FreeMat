function A = jit_test044
  A = zeros(1,1000000);
  for i=1:1000000
    A(i) = min(max(i,200),10000);
  end
