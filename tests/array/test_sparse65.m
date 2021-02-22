% Test sparse matrix array vector-subset assignment
function x = test_sparse65
[yi1,zi1] = sparse_test_mat('int32',300,400);
[yf1,zf1] = sparse_test_mat('float',300,400);
[yd1,zd1] = sparse_test_mat('double',300,400);
[yc1,zc1] = sparse_test_mat('complex',300,400);
[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
ndx = randi(ones(1500,1),300*400*ones(1500,1));
gi = int32(100*randn(1500,1));
gf = float(randn(1500,1));
gd = randn(1500,1);
gc = complex(randn(1500,1)+i*randn(1500,1));
gz = dcomplex(randn(1500,1)+i*randn(1500,1));
yi1(ndx) = gi; zi1(ndx) = gi;
yf1(ndx) = gf; zf1(ndx) = gf;
yd1(ndx) = gd; zd1(ndx) = gd;
yc1(ndx) = gc; zc1(ndx) = gc;
yz1(ndx) = gz; zz1(ndx) = gz;
% Cannot use testeq because if we set the same element to two
% different values, the result is different for full versus sparse
% matrices
x = testeq2(yi1,zi1,ndx) & testeq2(yf1,zf1,ndx) & testeq2(yd1,zd1,ndx) & testeq2(yc1,zc1,ndx) & testeq2(yz1,zz1,ndx);

function n = length(x)
n = prod(size(x));

function x = testeq2(a,b,ndx)
k = find(a-b);
x = 1;
for i = 1:length(k)
  x = x & length(find(ndx == k(i)));
end
