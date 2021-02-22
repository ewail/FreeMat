function A=tjit10
A = zeros(512);
for i=1:512
  for j=1:512
    A(i,j) = abs(i-j);
  end
end
