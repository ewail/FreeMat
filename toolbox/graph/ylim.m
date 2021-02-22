% DOCBLOCK handle_ylim

%Copyright (c) 2004,2006 Brian Yanoff, Samit Basu
% Licensed under the GPL
function ret = ylim(varargin)
  if ((nargin > 0) && isnumeric(varargin{1}) && isscalar(varargin{1}) && ishandle(varargin{1}))
    handle = varargin{1};
    varargin(1) = [];
    nargin = nargin - 1;
  else
    handle = gca;
  end
  if (nargin == 0)
    ret = get(handle,'ylim');
    return;
  end
  if (isstr(varargin{1}))
    if (strcmp(varargin{1},'mode'))
      ret = get(handle,'ylimmode');
      return;
    end
    if (strcmp(varargin{1},'auto'))
      set(handle,'ylimmode','auto');
      return;
    end
    if (strcmp(varargin{1},'manual'))
      set(handle,'ylimmode','manual');
      return;
    end
    error('Unrecognized argument to ylim');
  elseif (isnumeric(varargin{1}) && numel(varargin{1})==2)
    set(gca,'ylim',varargin{1});
  else
    error('Unrecognized argument to ylim');
  end
