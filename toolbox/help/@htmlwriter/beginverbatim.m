
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function beginverbatim(&p,text)
  fprintf(p.myfile,'<PRE>\n');
  p.verbatim = true;
