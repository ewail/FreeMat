% DOCBLOCK sparse_speye

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function a = speye(n,m)
if (nargin == 1)
  m = n;
end
if ((m <= 0) | (n <= 0))
  error('size arguments to speye function must be positive integers');
end
a = sparse(1:m,1:n,1);
