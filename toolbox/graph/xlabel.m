% DOCBLOCK handle_xlabel

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function h = xlabel(varargin)
  if (nargin < 1) 
    error('xlabel needs at least one argument')
  end
  if (nargout > 1) 
    error('xlabel outputs at most one argument')
  end
  h1 = htext('string',varargin{1},'parent',gca,'horiz','center','vert','middle','autoparent','off',varargin{2:end});
  set(gca,'xlabel',h1);
  if (nargout == 1) 
    h = h1;
  end
