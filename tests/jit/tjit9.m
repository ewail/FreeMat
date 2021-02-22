function A = tjit9
A = zeros(5);
for i=1:500
  for j=1:500
    A(i,j) = i-j;
  end
end
