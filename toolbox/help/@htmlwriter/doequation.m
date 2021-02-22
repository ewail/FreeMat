
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function doequation(&p,eqn)
  p.eqnlist = [p.eqnlist,{eqn}];
  fprintf(p.myfile,'<P>\n');
  fprintf(p.myfile,'<DIV ALIGN="CENTER">\n');
  fprintf(p.myfile,'<IMG SRC="%s_eqn%d.png">\n',p.modulename, ...
          numel(p.eqnlist));
  fprintf(p.myfile,'</DIV>\n');
  fprintf(p.myfile,'<P>\n');
  
