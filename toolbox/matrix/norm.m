% DOCBLOCK array_norm
% Copyright (c) 2005 Samit Basu
% Licensed under the GPL
function y = norm(A,p)
if (~isset('p'))
  p = 2;
end
if (isempty(A))
  y = 0;
  return;
end
if (min(size(A)) == 1)
  y = vector_norm(A,p);
else
  y = matrix_norm(A,p);
end

function y = vector_norm(A,p)
if (p<1 && p~=-inf) 
  error('p argument in vector norm must be between 1 and inf, or be -inf');
end
if (p == inf)
  y = max(abs(A));
elseif (p == -inf)
  y = min(abs(A));
elseif ((p == 2) || strcmp(p,'fro'))
  y = xnrm2(A);
else
  y = sum(abs(A).^p)^(1/p);
end

function y = matrix_norm(A,p)
switch p
  case 1
     y = max(sum(abs(A)));
  case 2
     s = svd(A);
     y = s(1);
  case inf
     y = max(sum(abs(A')));
  case 'fro'
     y = xnrm2(A);
  otherwise
     error('unrecognized norm p-argument');
end


