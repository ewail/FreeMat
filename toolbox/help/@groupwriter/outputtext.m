
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function outputtext(&p,text)
  for i=1:numel(p.clients)
    outputtext(p.clients{i},text);
  end
