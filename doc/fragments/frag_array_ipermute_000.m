0
A = randn(13,5,7,2);
size(A)
B = permute(A,[3,4,2,1]);
size(B)
C = ipermute(B,[3,4,2,1]);
size(C)
any(A~=C)
