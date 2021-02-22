function A = jit_test048
  A = zeros(1,1000000);
  for i=1:10:100000
    A(i + (1:8) - 1) = 1:8;
  end
