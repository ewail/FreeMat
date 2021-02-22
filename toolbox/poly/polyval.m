% DOCBLOCK curvefit_polyval

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = polyval(p,x)
y = p(1);
n = prod(size(p));
for i = 2:n
  y = y.*x + p(i);
end
