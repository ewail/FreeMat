function A = jit_test004
  A = zeros(512);
  for i=1:512
    for j=1:512
      k = i-j;
      if ((k < 5) && (k>-5))
         A(i,j) = k;
      else
         A(i,j) = i+j;
      end
    end
  end
