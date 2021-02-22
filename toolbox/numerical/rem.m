% DOCBLOCK mathfunctions_rem

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = rem(x,n)
  if (isscalar(x))
    x = repmat(x,size(n));
  end
  if (isscalar(n))
    n = repmat(n,size(x));
  end
  m = fix(x./n);
  y = x - m.*n;
