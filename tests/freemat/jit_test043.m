function A = jit_test043
  M = zeros(512,512);
  A = 0;
  for i=1:8:64
    for j=1:8:512
      L = M(i+(0:7),j+(0:7));
      P = [1;4;2;3;5;6;7;8];
      L*P + 3;
      A = A + 3;
    end
  end
