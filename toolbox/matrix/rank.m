% DOCBLOCK array_rank
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = rank(A,tol)
if (isempty(A))
  y = 0;
  return;
end
s = svd(A);
if (~isset('tol'))
  tol = max(size(A))*s(1)*teps(A);
end
y = sum(s>tol);

