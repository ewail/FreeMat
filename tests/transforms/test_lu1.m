% Test the LU decomposition with 2 argument return for full matrices
function x = test_lu1
A = float(randn(20));
[L,U] = lu(A);
x = testeq(100,L*U,A);
A = double(randn(20));
[L,U] = lu(A);
x = x & testeq(100,L*U,A);
A = complex(randn(20)+i*randn(20));
[L,U] = lu(A);
x = x & testeq(100,L*U,A);
A = dcomplex(randn(20)+i*randn(20));
[L,U] = lu(A);
x = x & testeq(100,L*U,A);

function x = testeq(tmag,a,b)
  d = full(a)-full(b);
  if (strcmp(typeof(d),'double') | strcmp(typeof(d),'dcomplex'))
    x = isempty(find(abs(d)>tmag*eps));
  else
    x = isempty(find(abs(d)>tmag*feps));
  end
