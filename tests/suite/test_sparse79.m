% Test sparse-sparse matrix array subtract
function x = test_sparse79
[yi1,zi1] = sparse_test_mat('int32',300,400);
[yf1,zf1] = sparse_test_mat('float',300,400);
[yd1,zd1] = sparse_test_mat('double',300,400);
[yc1,zc1] = sparse_test_mat('complex',300,400);
[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
[yi2,zi2] = sparse_test_mat('int32',300,400);
[yf2,zf2] = sparse_test_mat('float',300,400);
[yd2,zd2] = sparse_test_mat('double',300,400);
[yc2,zc2] = sparse_test_mat('complex',300,400);
[yz2,zz2] = sparse_test_mat('dcomplex',300,400);
x = testeq(yi1-yi2,zi1-zi2) & testeq(yf1-yf2,zf1-zf2) & testeq(yd1-yd2,zd1-zd2) & testeq(yc1-yc2,zc1-zc2) & testeq(yz1-yz2,zz1-zz2);
