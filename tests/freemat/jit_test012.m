function A = jit_test012
  A = zeros(400);
  for i=1:500
    for j=1:500;
      A(i,j) = i-j;
    end
  end
