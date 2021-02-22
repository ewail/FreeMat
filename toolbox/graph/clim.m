% DOCBLOCK handle_clim

%Copyright (c) 2004,2006 Brian Yanoff, Samit Basu
% Licensed under the GPL

function ret = clim(varargin)
  if ((nargin > 0) && isnumeric(varargin{1}) && isscalar(varargin{1}) && ishandle(varargin{1}))
    handle = varargin{1};
    varargin(1) = [];
    nargin = nargin - 1;
  else
    handle = gca;
  end
  if (nargin == 0)
    ret = get(handle,'clim');
    return;
  end
  if (isstr(varargin{1}))
    if (strcmp(varargin{1},'mode'))
      ret = get(handle,'climmode');
      return;
    end
    if (strcmp(varargin{1},'auto'))
      set(handle,'climmode','auto');
      return;
    end
    if (strcmp(varargin{1},'manual'))
      set(handle,'climmode','manual');
      return;
    end
    error('Unrecognized argument to clim');
  elseif (isnumeric(varargin{1}) && numel(varargin{1})==2)
    set(gca,'clim',varargin{1});
  else
    error('Unrecognized argument to clim');
  end
