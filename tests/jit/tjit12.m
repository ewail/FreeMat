function res=tjit12
for i=1:5
  res = 0;
  for j=0:20000
    res = res + sin(j);
    res = res + cos(j);
  end
end

