% Test the QR decomposition (without pivoting, minimal decomposition, dcomplex)
function test_val = test_qr4
a = double([1,2,3;4,5,6;7,8,9;10,0,5]);
a = a + i*(a-1);
% float precision
[q,r] = qr(a,0);
qorg = q'*q - diag(ones(1,3));
t1 = (max(abs(qorg(:)))<1e-15);
t2 = ((size(q,1) == 4) & (size(q,2) == 3));
t3 = ((size(r,1) == 3) & (size(r,2) == 3));
aorg = q*r - a;
t4 = (max(abs(qorg(:)))<1e-15);
t5 = max(diag(r,-1)) == 0;
t6 = max(diag(r,-2)) == 0;
test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
