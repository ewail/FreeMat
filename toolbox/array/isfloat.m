% DOCBLOCK array_isfloat

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = isfloat(y)
  x = any(strcmp({'single','double'},class(y)));
  
  
