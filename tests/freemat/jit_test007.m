function A = jit_test007
  B = ones(1,100000);
  j = 0;
  for i=1:100000
    j = j + B(1,i);
  end
  A = j;
