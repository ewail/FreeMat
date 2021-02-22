function B = jit_test053
  A = zeros(1,1000000);
  B = zeros(1,1000000);
  for i=1:100000
    p = (i-50000)/5000;
    A(i) = log(p)+1;
    B(i) = A(i);
  end
