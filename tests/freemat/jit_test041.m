function A = jit_test041
  A = [];
  for i=1:10000
    A = [1,2,3,4;5,4,3,2;8,9,4,5];
    T = [1,2;3,4];
    Y = [8;7];
    pol = Y'*(T*Y);
  end
