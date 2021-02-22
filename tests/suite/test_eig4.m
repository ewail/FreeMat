% Test generalized eigenvalue function - general matrices 
function t = test_eig4
% First the float version
t1all = 1;
for i=2:4:100
  a = float(randn(i)); 
  b = float(randn(i)); 
  [v,d] = eig(a,b);
  emat = a*v - b*v*d;
  er = max(abs(emat(:)));
  bnd = 8*max(diag(abs(d)))*feps*i;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
  g = eig(a,b);
  e2 = max(abs(sort(g)-sort(diag(d))));
  tb = e2<bnd;
  if (~tb) printf('float: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
  t1all = t1all & t1 & tb;
end
% Now the double version
t2all = 1;
for i=2:4:100
  a = double(randn(i)); 
  b = double(randn(i)); 
  [v,d] = eig(a,b);
  emat = a*v - b*v*d;
  er = max(abs(emat(:)));
  bnd = 8*max(diag(abs(d)))*eps*i;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
  g = eig(a,b);
  e2 = max(abs(sort(g)-sort(diag(d))));
  tb = e2<bnd;
  if (~tb) printf('double: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
  t2all = t2all & t1 & tb;
end
% Now the complex version
t3all = 1;
for i=2:4:100
  a = complex(randn(i)+j*randn(i)); 
  b = complex(randn(i)+j*randn(i)); 
  [v,d] = eig(a,b);
  emat = a*v - b*v*d;
  er = max(abs(emat(:)));
  bnd = 8*max(diag(abs(d)))*feps*i;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
  g = eig(a,b);
  e2 = max(abs(sort(g)-sort(diag(d))));
  tb = e2<bnd;
  if (~tb) printf('complex: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
  t3all = t3all & t1 & tb;
end
% Now the double version
t4all = 1;
for i=2:4:100
  a = dcomplex(randn(i)+j*randn(i)); 
  b = dcomplex(randn(i)+j*randn(i)); 
  [v,d] = eig(a,b);
  emat = a*v - b*v*d;
  er = max(abs(emat(:)));
  bnd = 8*max(diag(abs(d)))*eps*i;
  t1 = (er < bnd);
  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
  g = eig(a,b);
  e2 = max(abs(sort(g)-sort(diag(d))));
  tb = e2<bnd;
  if (~tb) printf('dcomplex: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
  t4all = t4all & t1 & tb;
end
t = t1all & t2all & t3all & t4all;
