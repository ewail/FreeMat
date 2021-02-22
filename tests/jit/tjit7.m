function A = tjit7
A = 1:10000;
B = zeros(1,10000);
for i=1:10000; 
  B(i) = A(i); 
end
