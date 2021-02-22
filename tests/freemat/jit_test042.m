function A = jit_test042
  M = zeros(512,512);
  A = zeros(1,10000);
  for i=1:8:512
    for j=1:8:512
      L = M(i+(0:7),j+(0:7));
      P = [1;4;2;3;5;6;7;8];
      Q = L*P;
      A(i+(0:7)) = Q;
    end
  end
