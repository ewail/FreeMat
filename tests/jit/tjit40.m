function C = tjit40
A = ones(1,100000);
B = zeros(1,100000);
C = zeros(1,100000);
for i=1:100000
  C(i) = A(i) || B(i);
end
