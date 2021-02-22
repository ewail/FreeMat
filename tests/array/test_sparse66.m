% Test sparse matrix array ndim-subset assignment
function x = test_sparse66
[yi1,zi1] = sparse_test_mat('int32',300,400);
[yf1,zf1] = sparse_test_mat('float',300,400);
[yd1,zd1] = sparse_test_mat('double',300,400);
[yc1,zc1] = sparse_test_mat('complex',300,400);
[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
ndxr = randi(ones(100,1),300*ones(100,1));
ndxc = randi(ones(100,1),400*ones(100,1));
gi = int32(100*randn(100,100));
gf = float(randn(100,100));
gd = randn(100,100);
gc = complex(randn(100,100)+i*randn(100,100));
gz = dcomplex(randn(100,100)+i*randn(100,100));
yi1(ndxr,ndxc) = gi; zi1(ndxr,ndxc) = gi;
yf1(ndxr,ndxc) = gf; zf1(ndxr,ndxc) = gf;
yd1(ndxr,ndxc) = gd; zd1(ndxr,ndxc) = gd;
yc1(ndxr,ndxc) = gc; zc1(ndxr,ndxc) = gc;
yz1(ndxr,ndxc) = gz; zz1(ndxr,ndxc) = gz;
x = testeq(yi1,zi1) & testeq(yf1,zf1) & testeq(yd1,zd1) & testeq(yc1,zc1) & testeq(yz1,zz1);
