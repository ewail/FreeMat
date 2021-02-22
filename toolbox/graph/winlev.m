% DOCBLOCK handle_winlev

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function [win,lev] = winlev(window,level)
g = gca;
if (nargin == 2)
  set(g,'clim',[level-window/2,level+window/2]);
else
  clim = get(g,'clim');
  win = clim(2)-clim(1);
  lev = (clim(2)+clim(1))/2;
end

  
