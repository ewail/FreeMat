
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function outputtext(&p,text)
  if (p.ignore) return; end
  if (p.verbatim)
    fprintf(p.myfile,'%s',text);
  else
    fprintf(p.myfile,'%s',expand_codes(text));
  end
  
