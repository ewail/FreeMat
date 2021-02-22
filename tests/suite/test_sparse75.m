% Test sparse matrix array finite test
function x = test_sparse75
[yi1,zi1] = sparse_test_mat('int32',300,400);
[yf1,zf1] = sparse_test_mat('float',300,400);
[yd1,zd1] = sparse_test_mat('double',300,400);
[yc1,zc1] = sparse_test_mat('complex',300,400);
[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
x = 1;
yi1(150,200) = inf;
try
  [l,u] = lu(yi1);
  x = 0;
catch
  x = x & 1;
end
try
  [l,u] = lu(yf1);
  x = 0;
catch
  x = x & 1;
end
try
  [l,u] = lu(yd1);
  x = 0;
catch
  x = x & 1;
end
try
  [l,u] = lu(yc1);
  x = 0;
catch
  x = x & 1;
end
try
  [l,u] = lu(yz1);
  x = 0;
catch
  x = x & 1;
end
