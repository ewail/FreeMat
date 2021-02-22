
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function endverbatim(&p)
  if (p.ignore); return; end;
  fprintf(p.myfile,'\n');
  p.verbatim = false;
  
