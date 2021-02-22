
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function doitemize(&p,enums)
  fprintf(p.myfile,'\\begin{itemize}\n');
  for i=1:numel(enums)
    fprintf(p.myfile,'\\item %s\n',expand_codes(enums{i}));
  end
  fprintf(p.myfile,'\\end{itemize}\n');
  
