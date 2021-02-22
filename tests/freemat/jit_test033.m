function A = jit_test033
  A = zeros(100000,1);
  for i=1:90000
   A(i+(1:2:50),1) = A(i+(1:2:50),1) + 6;
  end
