
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function dofile(&p,filename,text)
  for i = 1:numel(p.clients)
    dofile(p.clients{i},filename,text);
  end
