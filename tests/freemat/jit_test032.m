function A = jit_test032
  A = zeros(10000,2);
  B = [3,4;5,1];
  for i=1:10000
    A(i,1:2) = [1,i] / B;
  end
