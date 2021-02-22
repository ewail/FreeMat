0
A = sparse(1000,1000);
for i=1:25; A((1:40) + 40*(i-1),(1:40) + 40*(i-1)) = 1; end;
spy(A,'gx')
mprint spy2
