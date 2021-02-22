% DOCBLOCK mathfunctions_coth

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = coth(x)
  y = 1.0 ./ tanh(x);
  
