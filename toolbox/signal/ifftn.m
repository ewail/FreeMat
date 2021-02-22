% DOCBLOCK transforms_ifftn

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = ifftn(x,dims)
  if (nargin == 1)
    dims = size(x);
  end
  dims = dims(:)';
  if (numel(dims) < ndims(x))
    pnts = size(x);
    dims = [dims,pnts((numel(dims)+1):end)];
  end
  y = x;
  if (~isa(y,'single')) y = double(y); end;
  if (numel(y) == 0) return; end;
  for n=1:numel(dims)
    y = ifft(y,dims(n),n);
  end
