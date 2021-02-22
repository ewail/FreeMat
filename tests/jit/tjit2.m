function A = tjit2
A = zeros(1,1000);
j = 4;
p = 1f;
q = 0f;
for i=1:1000
  if (i > 50)
    k = 55;
  elseif (i > 40)
    k = 45;
  elseif (i > 30)
    k = 35;
  else
    k = 15;
  end
  A(i) = k;
end
