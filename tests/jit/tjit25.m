function A = tjit25
A = zeros(1,10000);
for j=1:10
  for i=1:10000;
    A(i) = tjit_sum(A(i),i);
    A(i) = tjit_sum(A(i),i);
  end
end

