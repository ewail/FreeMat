function C = tjit13
C = 0;
for j=1:1000;
  B = j;
  for i=1:10000
    C = C + B;
  end
end

