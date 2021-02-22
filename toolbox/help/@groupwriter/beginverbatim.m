
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function beginverbatim(&p)
  for i=1:numel(p.clients)
    beginverbatim(p.clients{i});
  end
