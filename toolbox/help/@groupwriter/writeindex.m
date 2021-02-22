
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function writeindex(&p)
  for i = 1:numel(p.clients)
    writeindex(p.clients{i});
  end
