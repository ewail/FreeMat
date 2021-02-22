
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function begingroup(&p,groupname)
  for i = 1:numel(p.clients)
    begingroup(p.clients{i},groupname);
  end
