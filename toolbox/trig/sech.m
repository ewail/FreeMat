% DOCBLOCK mathfunctions_sech

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = sech(x)
  y = 1.0 ./cosh(x);
  
