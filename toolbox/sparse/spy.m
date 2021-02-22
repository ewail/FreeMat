% DOCBLOCK sparse_spy

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function spy(A,colspec)
if (nargin==1)
  colspec = 'r.';
end
[i,j] = find(A);
plot(i,j,colspec)
xlabel(sprintf('nz = %d',int32(nnz(A))));
set(gca,'ydir','reverse');



