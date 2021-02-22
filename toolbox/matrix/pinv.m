% DOCBLOCK array_pinv
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = pinv(A,tol)
if (isempty(A))
  y = A;
  return;
end
[u,s,v] = svd(A,0);
if (~isset('tol'))
  tol = max(size(A))*s(1,1)*teps(A);
end
sd = diag(s);
sd(sd > tol) = 1./(sd(sd > tol));
s = diag(sd);
y = v*s*u';
