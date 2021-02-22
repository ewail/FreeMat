
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function doitemize(&p,enums)
  for i = 1:numel(p.clients)
    doitemize(p.clients{i},enums);
  end
