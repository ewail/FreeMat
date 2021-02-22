function A = tjit3
A = zeros(1000,1000);
k = 5;
m = 0;
for i=1:1000
  for j=1:1000
    A(i,j) = i-j;
    m = i;
  end
end
