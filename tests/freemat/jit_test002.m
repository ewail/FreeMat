function A = jit_test002
  A = zeros(512);
  for i=1:512;
    for j=1:512;
       A(i,j) = i-j;
    end
  end
