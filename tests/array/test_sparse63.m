% Test sparse matrix array vector-subset extraction
function x = test_sparse63
[yi1,zi1] = sparse_test_mat('int32',300,400);
[yf1,zf1] = sparse_test_mat('float',300,400);
[yd1,zd1] = sparse_test_mat('double',300,400);
[yc1,zc1] = sparse_test_mat('complex',300,400);
[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
ndx = randi(ones(1500,1),300*400*ones(1500,1));
a1 = yi1(ndx); b1 = zi1(ndx);
a2 = yf1(ndx); b2 = zf1(ndx);
a3 = yd1(ndx); b3 = zd1(ndx);
a4 = yc1(ndx); b4 = zc1(ndx);
a5 = yz1(ndx); b5 = zz1(ndx);
x = testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
ndx = [];
a1 = yi1(ndx); b1 = zi1(ndx);
a2 = yf1(ndx); b2 = zf1(ndx);
a3 = yd1(ndx); b3 = zd1(ndx);
a4 = yc1(ndx); b4 = zc1(ndx);
a5 = yz1(ndx); b5 = zz1(ndx);
x = x & testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
