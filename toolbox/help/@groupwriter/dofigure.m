
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function dofigure(&p,figname)
  for i = 1:numel(p.clients)
    dofigure(p.clients{i},figname);
  end
