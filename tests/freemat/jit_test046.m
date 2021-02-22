function A = jit_test046
  A = zeros(1,1000000);
  for i=1:1000
    for j=1:1000
      A(i+10*j) = A(i+j);
    end
  end
