
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function doequation(&p,eqn)
  for i = 1:numel(p.clients)
    doequation(p.clients{i},eqn);
  end
