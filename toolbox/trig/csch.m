% DOCBLOCK mathfunctions_csch

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = csch(x)
  y = 1.0 ./ sinh(x);
  
