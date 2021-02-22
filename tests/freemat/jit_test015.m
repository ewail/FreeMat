function A = jit_test015
  A = 0;
  for i=0:200000
    A = A + abs(sin(i/200000*pi));
  end
