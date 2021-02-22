% DOCBLOCK mathfunctions_acosd

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acosd(x)
  y = rad2deg(acos(x));
  y(x == inf) = complex(0,inf);
  y(x == -inf) = complex(180,-inf);
