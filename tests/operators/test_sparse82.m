% Test sparse-sparse matrix array solution
function x = test_sparse82
[yd1,zd1] = sparse_test_mat('double',100,100);
[yz1,zz1] = sparse_test_mat('dcomplex',100,100);
[yd2,zd2] = sparse_test_mat('double',100,60);
[yz2,zz2] = sparse_test_mat('dcomplex',100,60);
x = testeq3(yd1\yd2,zd1\zd2) & testeq3(yz1\yz2,zz1\zz2);

function x = testeq3(a,b)
  k = abs(a-b);
  x = max(k(:)) < (size(a,2)*size(a,1)*eps*4);
