
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function dofile(&p,filename,textstring)
  fprintf(p.myfile,'\\begin{verbatim}\n');
  fprintf(p.myfile,'    %s\n',filename);
  fprintf(p.myfile,'%s',textstring);
  fprintf(p.myfile,'\\end{verbatim}\n');
  
