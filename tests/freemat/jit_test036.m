function A = jit_test036
  A = zeros(2,100000);
  B = zeros(5,100000);
  for i=1:10000
     for j=1:5
        B(j,i) = j+i;
     end;
     A(1,i) = min(B(1:5,i));
     A(2,i) = max(B(1:5,i));
  end
