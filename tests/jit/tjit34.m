function B = tjit34
A = zeros(1,100000);
B = zeros(1,100000);
c = (-9:9)';
for i=10:(100000-10)
  w = A(i+(-9:9));
  accum = 0;
  for j=-9:9
    ndx = j + 10;
    accum = accum + j*w(ndx);
  end
  b = accum;
  B(i) = b;
end
