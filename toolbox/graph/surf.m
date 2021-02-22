% DOCBLOCK handle_surf

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function ohandle = surf(varargin)
   % Check for an axes handle
   if (nargin>=2)
      if (isnumeric(varargin{1}) && (length(varargin{1})==1) && ...
         ishandle(varargin{1},'axes'))
         handle = varargin{1}(1);
         varargin(1) = [];
         nargin = nargin - 1;
      else   
         handle = newplot;
      end
else
   handle = newplot;
end
  saveca = gca;
  axes(handle);
  % search for the propertyname/value pairs
  propstart = 0;
  if (nargin > 2)
    propstart = nargin-1;
    while ((propstart >= 1) && isa(varargin{propstart},'char') && ...
    pvalid('line',varargin{propstart}))
      propstart = propstart - 2;
    end
    propstart = propstart + 2;
  end
  propset = {};
  if ((propstart > 0) && (propstart < nargin))
     propset = varargin(propstart:end);
     varargin(propstart:end) = [];
  end
  if (length(varargin) == 0)
    h = surface(propset{:});
  elseif (length(varargin) == 1)
    h = surface('zdata',varargin{1},propset{:});
  elseif (length(varargin) == 3)
    h = surface('xdata',varargin{1},'ydata',varargin{2},...
	'zdata',varargin{3},propset{:});
  elseif (length(varargin) == 4)
    h = surface('xdata',varargin{1},'ydata',varargin{2},...
	'zdata',varargin{3},'cdata',varargin{4},propset{:});
  else
    error('Unrecognized arguments to surf command');
  end
  % These lines break pcolor
  %  if ~ishold
  %    view( handle, 3 );
  %    grid;
  % end

axes(saveca);
if (nargout > 0)
    ohandle = h;
end

