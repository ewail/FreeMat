% Test sparse matrix array ndim-subset extraction
function x = test_sparse64
[yi1,zi1] = sparse_test_mat('int32',300,400);
[yf1,zf1] = sparse_test_mat('float',300,400);
[yd1,zd1] = sparse_test_mat('double',300,400);
[yc1,zc1] = sparse_test_mat('complex',300,400);
[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
row_ndx = randi(ones(150,1),300*ones(150,1));
col_ndx = randi(ones(150,1),400*ones(150,1));
a1 = yi1(row_ndx,col_ndx); b1 = zi1(row_ndx,col_ndx);
a2 = yf1(row_ndx,col_ndx); b2 = zf1(row_ndx,col_ndx);
a3 = yd1(row_ndx,col_ndx); b3 = zd1(row_ndx,col_ndx);
a4 = yc1(row_ndx,col_ndx); b4 = zc1(row_ndx,col_ndx);
a5 = yz1(row_ndx,col_ndx); b5 = zz1(row_ndx,col_ndx);
x = testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
row_ndx = [];
a1 = yi1(row_ndx,col_ndx); b1 = zi1(row_ndx,col_ndx);
a2 = yf1(row_ndx,col_ndx); b2 = zf1(row_ndx,col_ndx);
a3 = yd1(row_ndx,col_ndx); b3 = zd1(row_ndx,col_ndx);
a4 = yc1(row_ndx,col_ndx); b4 = zc1(row_ndx,col_ndx);
a5 = yz1(row_ndx,col_ndx); b5 = zz1(row_ndx,col_ndx);
x = x & testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
col_ndx = [];
a1 = yi1(row_ndx,col_ndx); b1 = zi1(row_ndx,col_ndx);
a2 = yf1(row_ndx,col_ndx); b2 = zf1(row_ndx,col_ndx);
a3 = yd1(row_ndx,col_ndx); b3 = zd1(row_ndx,col_ndx);
a4 = yc1(row_ndx,col_ndx); b4 = zc1(row_ndx,col_ndx);
a5 = yz1(row_ndx,col_ndx); b5 = zz1(row_ndx,col_ndx);
x = x & testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
