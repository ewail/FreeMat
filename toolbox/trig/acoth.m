% DOCBLOCK mathfunctions_acoth


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acoth(x)
  y = atanh(1 ./ x);
  
