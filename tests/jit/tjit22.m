function A = tjit22
A = zeros(1000);
for i=1:1000
  for j=1:1000
    A(j,i) = i-j;
  end
end


