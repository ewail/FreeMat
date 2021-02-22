function C = tjit44
N = 10000;
A = ones(1,N);
C = zeros(1,N);
for i=1:N;
  C(i) = any(A(i));
end
