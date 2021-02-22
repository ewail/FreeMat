function A = jit_test013
  A = zeros(512);
  for i=1:512
    for j=1:512
      A(i,j) = abs(i-j);
    end
  end
