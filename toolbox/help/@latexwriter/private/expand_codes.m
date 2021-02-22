
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function text = expand_codes(text)
  text = regexprep(text,'\@\|([^\|]*)\|','\verb|$1|');
