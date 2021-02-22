function A = tjit33
A = zeros(512);
offset = 3;
for k=1:2
  for i=1:512
    for j=1:512
      A(i,j) = i-j+offset;
    end
  end
end

