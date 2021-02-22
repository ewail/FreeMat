function A = jit_test031
  A = zeros(2,10000);
  B = [3,4;5,1];
  C = [1;0];
  for i=1:10000
    A(1:2,i) = B \ [1;i];
  end
