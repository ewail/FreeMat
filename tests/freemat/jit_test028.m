function A = jit_test028
  A = zeros(10000,1);
  B = [1;2;3];
  C = [4;5;6];
  for i=1:9000
    A(i+(0:5),1) = [B;C];
  end
