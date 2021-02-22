% DOCBLOCK string_isdigit

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = isdigit(s)
  s = int32(string(s));
  x = ((s >=48 ) & (s <= 57));
  
