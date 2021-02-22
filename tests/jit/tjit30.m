function A = tjit30
A = zeros(3000);
for i=1:3000
  b = A(1:i);
  A(1:i) = A(1:i) + b;
end
