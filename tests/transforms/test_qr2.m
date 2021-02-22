% Test the QR decomposition (without pivoting, minimal decomposition, float)
function test_val = test_qr2
a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
% float precision
[q,r] = qr(a,0);
qorg = q'*q - diag(ones(1,3));
t1 = (max(abs(qorg(:)))<1e-6);
t2 = ((size(q,1) == 4) & (size(q,2) == 3));
t3 = ((size(r,1) == 3) & (size(r,2) == 3));
aorg = q*r - a;
t4 = (max(abs(qorg(:)))<1e-6);
t5 = max(diag(r,-1)) == 0;
t6 = max(diag(r,-2)) == 0;
test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
