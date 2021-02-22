% DOCBLOCK string_isalpha

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = isalpha(s)
  s = int32(string(s));
  x = (((s >= 65 ) & (s <= 90)) | (s >= 97) & (s <= 122));
  
  
