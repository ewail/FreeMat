% DOCBLOCK inspection_isvector

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = isvector(x)
  s = size(x);
  y = (prod(s) == (s(1)*s(2))) && ((s(1) == 1) || (s(2) == 1));
  
  
