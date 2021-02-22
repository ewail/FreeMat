% DOCBLOCK mathfunctions_acsch

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acsch(x)
  y = asinh(1 ./ x);
  
