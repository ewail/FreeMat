function A = tjit14
N = 512;
A = zeros(N);
for i=1:N
  for j=1:N
    A(i,j) = i-j;
  end
end

