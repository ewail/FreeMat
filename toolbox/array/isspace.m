% DOCBLOCK string_isspace

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = isspace(s)
  s = int32(string(s));
  x = ((s >=9 ) & (s <= 13)) | (s == 32);
  
