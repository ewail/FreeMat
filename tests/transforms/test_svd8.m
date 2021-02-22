% Test the full version of the svd with dcomplex vectors
function test_val = test_svd8
  t1all = 1;
  p = [];
  for n=2:4:100
    a = double(rand(n,2*n))+i*double(rand(n,2*n));
    [u,s,v] = svd(a);
    emat = abs(a-u*s*v');
    err = max(emat(:));
    p(n) = err;
    bnd = 10*max(abs(diag(s)))*eps*n;
    t1 = (err < bnd);
    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
    t1all = t1all & t1;    
  end
  for n=2:4:100
    a = double(rand(2*n,n))+i*double(rand(2*n,n));
    [u,s,v] = svd(a);
    emat = abs(a-u*s*v');
    err = max(emat(:));
    bnd = 10*max(abs(diag(s)))*eps*n;
    t1 = (err < bnd);
    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
    t1all = t1all & t1;    
  end
  for n=2:4:100
    a = double(rand(n,n))+i*double(rand(n,n));
    [u,s,v] = svd(a);
    emat = abs(a-u*s*v');
    err = max(emat(:));
    bnd = 10*max(abs(diag(s)))*eps*n;
    t1 = (err < bnd);
    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
    t1all = t1all & t1;    
  end
  test_val = t1all;
