function C = tjit42
N = 10000;
A = ones(1,N);
B = zeros(1,N);
C = zeros(1,N);
for i=1:N/2-5;
  C(i) = any(A(i:2:2*i));
end
