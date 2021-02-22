function A = jit_test026
  B = [3,4;5,2];
  C = [2;5];
  A = zeros(2,10000);
  for i=1:10000
    C(2,1) = i;
    A(1:2,i) = B*C + 1;
  end
