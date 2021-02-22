% DOCBLOCK handle_zlabel

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function h = zlabel(varargin)
  if (nargin < 1) 
    error('zlabel needs at least one argument')
  end
  h1 = htext('string',varargin{1},'parent',gca,'horiz','center','vert','middle','autoparent','off',varargin{2:end});
  set(gca,'zlabel',h1);
  if (nargout == 1) 
    h = h1;
  end
