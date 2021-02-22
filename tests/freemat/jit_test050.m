function A = jit_test050
  A = zeros(1,1000000);
  for i=1:10:100000
    A((1:8) + i) = i*3-2;
  end
