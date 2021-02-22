
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function doenumerate(&p,itemlist)
  fprintf(p.myfile,'<OL>\n');
  for i=1:numel(itemlist)
    fprintf(p.myfile,'<LI> %s </LI>\n',expand_codes(latin_filter(itemlist{i})));
  end
  fprintf(p.myfile,'</OL>\n');
