% Test sparse matrix array diagonal extraction
function x = test_sparse74
[yi1,zi1] = sparse_test_mat('int32',300,400);
[yf1,zf1] = sparse_test_mat('float',300,400);
[yd1,zd1] = sparse_test_mat('double',300,400);
[yc1,zc1] = sparse_test_mat('complex',300,400);
[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
x = testeq(diag(yi1,30),diag(zi1,30)) & testeq(diag(yf1,30),diag(zf1,30)) & testeq(diag(yd1,30),diag(zd1,30)) & testeq(diag(yc1,30),diag(zc1,30)) & testeq(diag(yz1,30),diag(zz1,30));
