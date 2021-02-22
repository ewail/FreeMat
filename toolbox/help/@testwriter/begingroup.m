
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function begingroup(&p,groupname)
  p.groupname = groupname;
  if (~strcmp(lower(groupname),'tests'))
    p.ignore = 1;
  else
    p.ignore = 0;
  end
