% Test the 'nobalance' option for eig
function t = test_eig3
B = [3,-2,-.9,2*eps;-2,4,1,-eps;-eps/4,eps/2,-1,0;-.5,-.5,.1,1];
[VN,DN] = eig(B,'nobalance');
er = B*VN - VN*DN;
er = max(abs(er(:)));
bnd = 1.2*max(diag(DN))*eps*8;
t = (er < bnd);
