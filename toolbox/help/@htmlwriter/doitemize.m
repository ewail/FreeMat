
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function doitemize(&p,enums)
  fprintf(p.myfile,'<UL>\n');
  for i=1:numel(enums)
    fprintf(p.myfile,'<LI> %s </LI>\n',expand_codes(latin_filter(enums{i})));
  end
  fprintf(p.myfile,'</UL>\n');
  
