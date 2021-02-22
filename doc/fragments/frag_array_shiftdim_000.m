0
x = uint8(10*randn(1,1,1,3,2));
[y,n] = shiftdim(x);
n
size(y)
c = shiftdim(y,-n);
size(c)
any(c~=x)
