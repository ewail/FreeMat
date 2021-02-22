function A = jit_test038
  A = zeros(512,512);
  for i=1:8:64
    for j=1:8:512
      A(i+(0:7),j+(0:7)) = j;
    end
  end
