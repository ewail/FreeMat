
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function begingroup(&p,groupname)
  p.groupname = groupname;
  if (~strcmp(lower(groupname),'usage'))
    p.ignore = 1;
  else
    fprintf(p.myfile,'Usage\n\n');
    p.ignore = 0;
  end
