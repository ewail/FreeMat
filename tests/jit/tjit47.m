function C = tjit47
N = 10000;
A = ones(1,N);
C = zeros(1,N);
w = [1,2];
for i=1:N-1;
  C(i) = w*([A(i);A(i)]);
end
