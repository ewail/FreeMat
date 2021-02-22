
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function endverbatim(&p)
  fprintf(p.myfile,'</PRE>\n<P>\n');
  p.verbatim = false;
  
