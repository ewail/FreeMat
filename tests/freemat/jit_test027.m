function A = jit_test027
  A = zeros(1,10000);
  B = [1,2,3];
  C = [4,5,6];
  for i=1:9000
    A(1,i+(0:5)) = [B,C];
  end
