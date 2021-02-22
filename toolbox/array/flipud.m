% DOCBLOCK array_flipud
% Copyright (c) 2002, 2003 Samit Basu
% Licensed under the GPL

function x = flipud(y)
  x = flipdim(y,1);
