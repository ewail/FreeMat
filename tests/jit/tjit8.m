function A = tjit8
A = zeros(1,10000);
B = 5;
for i=1:10000;
  C = B + i;
  A(i) = C;
end

