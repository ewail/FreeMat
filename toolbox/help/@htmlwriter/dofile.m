
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function dofile(&p,filename,textstring)
  if (p.ignore), return; end;
  fprintf(p.myfile,'<P>\n<PRE>\n');
  fprintf(p.myfile,'     %s\n',filename);
  fprintf(p.myfile,'%s\n',latin_filter(textstring));
  fprintf(p.myfile,'</PRE>\n');
  fprintf(p.myfile,'<P>\n');
