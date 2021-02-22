% Test DeleteSparseMatrix function
function x = test_sparse56
xi = sparse_test_mat('int32',100);
xf = sparse_test_mat('float',100);
xd = sparse_test_mat('double',100);
xc = sparse_test_mat('complex',100);
xz = sparse_test_mat('dcomplex',100);
xi = [];
xf = [];
xd = [];
xc = [];
xz = [];
x = isempty(xi) & isempty(xf) & isempty(xd) & isempty(xc) & isempty(xz);
