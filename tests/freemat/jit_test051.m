function A = jit_test051
  A = zeros(512,512);
  for i=1:8:512
    for j=1:8:512
      B(i+(0:7),j+(0:7)) = j;
    end
  end
