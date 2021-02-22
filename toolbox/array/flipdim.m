% DOCBLOCK array_flipdim

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = flipdim(y,n)
  if (isempty(y)), x = y; return; end
  szey = size(y);
  ndim = prod(size(szey));
  n = int32(n);
  if (n<1 | n>ndim), x = y; return; end
  d = {};
  for k=1:ndim
    if (k==n)
      d{k} = szey(k):-1:1;
    else
      d{k} = 1:szey(k);
    end
  end
  x = y(d{:});
