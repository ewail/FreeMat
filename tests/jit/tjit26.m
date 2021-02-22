function A = tjit26
A = zeros(1,10000);
for j=1:10
  for i=1:10000;
    A(i) = tjit_sum26(A(i),i);
    A(i) = tjit_sum26(A(i),i);
  end
end

function  y = tjit_sum26(a,b)
  y = a + tjit_double26(b);

function y = tjit_double26(a)
  y = a*2;

