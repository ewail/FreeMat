% Test sparse matrix individual element retrieval
function x = test_sparse67
[yi1,zi1] = sparse_test_mat('int32',300,400);
[yf1,zf1] = sparse_test_mat('float',300,400);
[yd1,zd1] = sparse_test_mat('double',300,400);
[yc1,zc1] = sparse_test_mat('complex',300,400);
[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
x = testeq(yi1(150,200),zi1(150,200)) & testeq(yf1(150,200),zf1(150,200)) & testeq(yd1(150,200),zd1(150,200)) & testeq(yc1(150,200),zc1(150,200)) & testeq(yz1(150,200),zz1(150,200));
