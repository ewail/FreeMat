% DOCBLOCK inspection_ismatrix

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = ismatrix(x)
  y = (ndims(x)==2);
  
  
