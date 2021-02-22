function A = jit_test037
  A = zeros(5,100000);
  for i=1:10000
    for j=1:5
      A(j,i) = (j-2.5)/2.5*0.87;
    end;
    A(1:5,i) = cos(A(1:5,i));
  end
