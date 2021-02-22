% Test the LU decomposition with 3 argument return for full matrices
function x = test_lu2
A = float(randn(20));
[L,U,P] = lu(A);
x = testeq(100,L*U,P*A);
A = double(randn(20));
[L,U,P] = lu(A);
x = x & testeq(100,L*U,P*A);
A = complex(randn(20)+i*randn(20));
[L,U,P] = lu(A);
x = x & testeq(100,L*U,P*A);
A = dcomplex(randn(20)+i*randn(20));
[L,U,P] = lu(A);
x = x & testeq(100,L*U,P*A);

function x = testeq(tmag,a,b)
  d = full(a)-full(b);
  if (strcmp(typeof(d),'double') | strcmp(typeof(d),'dcomplex'))
    x = isempty(find(abs(d)>tmag*eps));
  else
    x = isempty(find(abs(d)>tmag*feps));
  end
