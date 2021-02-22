function A = jit_test040
  A = zeros(1,1000000);
  for i=1:1000000;
    A(i) = -1;
  end;
