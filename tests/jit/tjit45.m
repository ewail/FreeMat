function C = tjit45
N = 10000;
A = ones(1,N);
C = zeros(1,N);
w = [1,2];
for i=1:N-1;
  C(i) = w*(A(1,i:i+1)');
end
