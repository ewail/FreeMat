% DOCBLOCK sparse_sprand

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = sprand(x,n,density)
if (nargin == 1)
  [i,j] = find(x);
  s = rand(size(i));
  y = sparse(i,j,s);
else
  m = round(x(1));
  n = round(n(1));
  density = max(0.0,min(1.0,density(1)));
  cnt = round(density*m*n);
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
  s = rand(size(i));
  y = sparse(i,j,s,m,n);
end
 
 
