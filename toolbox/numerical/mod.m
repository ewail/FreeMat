% DOCBLOCK mathfunctions_mod
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function y = mod(x,n)
  if (isscalar(x))
    x = repmat(x,size(n));
  end
  if (isscalar(n))
    n = repmat(n,size(x));
  end
  m = floor(x./n);
  y = x - m.*n;
  y(n==0) = x(n==0);
