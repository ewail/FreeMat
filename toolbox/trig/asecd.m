% DOCBLOCK mathfunctions_asecd

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = asecd(x)
  y = rad2deg(asec(x));
  y(x == 0) = complex(0,inf);
