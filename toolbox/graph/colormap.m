% DOCBLOCK handle_colormap

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function cmap = colormap(A)
if (nargin == 0)
  cmap = get(gcf,'colormap');
else
  set(gcf,'colormap',A);
end
