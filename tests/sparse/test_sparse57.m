% Test makeDenseArray function
function x = test_sparse57
[yi,zi] = sparse_test_mat('int32',400);
[yf,zf] = sparse_test_mat('float',400);
[yd,zd] = sparse_test_mat('double',400);
[yc,zc] = sparse_test_mat('complex',400);
[yz,zz] = sparse_test_mat('dcomplex',400);
x = testeq(full(yi),zi) & testeq(full(yf),zf) & testeq(full(yd),zd) & testeq(full(yc),zc) & testeq(full(yz),zz);
