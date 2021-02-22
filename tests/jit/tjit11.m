function A=tjit11
N = 500;
A = zeros(N,N);
for i=1:500
  for j=1:500
    A(j,i) = abs(i-j);
  end
end
