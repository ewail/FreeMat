function B = tjit31
A = zeros(1,100000);
B = zeros(1,100000);
c = (-9:9)';
for i=10:(100000-10)
  w = A(i+(-9:9));
  b = c*w;
  B(i) = b;
end
