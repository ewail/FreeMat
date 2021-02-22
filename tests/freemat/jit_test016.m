function A = jit_test016
  A = 0;
  for i=0:200000
    A = A + abs(cos(i/200000*pi));
  end
