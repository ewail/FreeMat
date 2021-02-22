function [y,z] = sparse_test_mat(typ, rows, cols)
  if (~exist('cols')) cols = rows; end
  a = rand(rows,cols);
  a(a>0.1) = 0;
  b = rand(rows,cols);
  b(b>0.1) = 0;
  switch(typ)
    case 'int32'
      z = (int32(100*a));
    case 'float'
      z = (float(a));
    case 'double'
      z = (double(a));
    case 'complex'
      z = (float(a) + i*float(b));
    case 'dcomplex'
      z = (a + i*b);
  end
  y = sparse(z);
