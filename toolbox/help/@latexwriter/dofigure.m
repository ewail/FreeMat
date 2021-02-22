
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function dofigure(&p,figname)
  fprintf(p.myfile,'\n\n');
  fprintf(p.myfile,'\\centerline{\\includegraphics[width=8cm]{%s}}\n\n', ...
          figname);
  
