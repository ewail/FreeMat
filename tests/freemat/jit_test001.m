function A = jit_test001
  A = zeros(1,100000);
  for i=1:100000; A(i) = i; end;
