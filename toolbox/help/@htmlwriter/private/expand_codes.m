
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function text = expand_codes(text)
  text = regexprep(text,'\@\|([^\|]*)\|','<code>$1</code>');
  if (strcmp(text,'\n')) 
     text = '<P>\n';
  end
