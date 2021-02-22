
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function dofigure(&p,figname)
  fprintf(p.myfile,'<P>\n');
  fprintf(p.myfile,'<DIV ALIGN="CENTER">\n');
  fprintf(p.myfile,'<IMG SRC="%s.png">\n',figname);
  fprintf(p.myfile,'</DIV>\n');
  fprintf(p.myfile,'<P>\n');
  
