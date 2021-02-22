
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function doenumerate(&p,enums)
  for i = 1:numel(p.clients)
    doenumerate(p.clients{i},enums);
  end
