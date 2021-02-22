% DOCBLOCK inspection_issquare

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = issquare(x)
  s = size(x);
  y = isempty(x) || (( s(1) == s(2) ) && ismatrix( x ));
  
  
