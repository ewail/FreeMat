
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function endverbatim(&p)
  fprintf(p.myfile,'\\end{verbatim}\n');
  p.verbatim = false;
  
