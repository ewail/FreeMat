function A=tjit1
A = zeros(512,512);
for i=1:512;
  for j=1:512
    k = i-j;
    if ((k<5) && (k>-5))
      A(i,j) = k;
    else
      A(i,j) = 0;
    end
  end
end
