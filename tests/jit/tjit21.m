function A = tjit21
A = zeros(1000);
for i=1:1000
  for j=1:1000
    A(i,j) = i-j;
  end
end

