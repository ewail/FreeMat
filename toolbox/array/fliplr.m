% DOCBLOCK array_fliplr

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function x = fliplr(y)
  x = flipdim(y,2);
