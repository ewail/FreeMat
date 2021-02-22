
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function beginverbatim(&p,text)
  if (p.ignore) return; end
  p.verbatim = true;
  fprintf(p.myfile,'\n');
