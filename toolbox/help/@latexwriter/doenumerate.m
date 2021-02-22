
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function doenumerate(&p,itemlist)
  fprintf(p.myfile,'\\begin{enumerate}\n');
  for i=1:numel(itemlist)
    fprintf(p.myfile,'\\item %s\n',expand_codes(itemlist{i}));
  end
  fprintf(p.myfile,'\\end{enumerate}\n');
  
