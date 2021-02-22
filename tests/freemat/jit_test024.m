function A = jit_test024
  A = zeros(1,10000);
  for j=1:10
    for i=1:10000;
      A(i) = tjit_sum(A(i),i);
     end;
  end;

function y = tjit_sum(a,b)
  y = a + tjit_double(b);

function y = tjit_double(a)
  y = a*2;
