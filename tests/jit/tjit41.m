function C = tjit41
N = 10000;
A = ones(1,N);
B = zeros(1,N);
C = zeros(1,N);
for i=1:N-1;
  C(i) = any(A(i:i+1));
end
