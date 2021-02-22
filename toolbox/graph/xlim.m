% DOCBLOCK handle_xlim

%Copyright (c) 2004,2006 Brian Yanoff, Samit Basu
% Licensed under the GPL

function ret = xlim(varargin)
  if ((nargin > 0) && isnumeric(varargin{1}) && isscalar(varargin{1}) && ishandle(varargin{1}))
    handle = varargin{1};
    varargin(1) = [];
    nargin = nargin - 1;
  else
    handle = gca;
  end
  if (nargin == 0)
    ret = get(handle,'xlim');
    return;
  end
  if (isstr(varargin{1}))
    if (strcmp(varargin{1},'mode'))
      ret = get(handle,'xlimmode');
      return;
    end
    if (strcmp(varargin{1},'auto'))
      set(handle,'xlimmode','auto');
      return;
    end
    if (strcmp(varargin{1},'manual'))
      set(handle,'xlimmode','manual');
      return;
    end
    error('Unrecognized argument to xlim');
  elseif (isnumeric(varargin{1}) && numel(varargin{1})==2)
    set(gca,'xlim',varargin{1});
  else
    error('Unrecognized argument to xlim');
  end
