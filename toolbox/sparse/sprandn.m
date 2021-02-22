% DOCBLOCK sparse_sprandn

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = sprandn(x,n,density)
if (nargin == 1)
  [i,j] = find(x);
  s = randn(size(i));
  y = sparse(i,j,s);
else
  m = round(x(1));
  n = round(n(1));
  density = max(0.0,min(1.0,density(1)));
  cnt = int32(density*m*n);
  if (cnt == 0)
    y = sparse(m,n);
    return;
  end
  i = randi(1,m*ones(cnt,1));
  j = randi(1,n*ones(cnt,1));
  A = [i,j];
  A = unique(A,'rows');
  i = A(:,1);
  j = A(:,2);
  s = randn(size(i));
  y = sparse(i,j,s,m,n);
end
 
 
