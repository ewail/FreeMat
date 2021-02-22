% DOCBLOCK handle_line

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = line(varargin)
  ax = gca;
  if (length(varargin) == 0), return; end
  % check for formal syntax
  if (isstr(varargin{1}))
    handle = hline(varargin{:});
%  elseif ((length(varargin) == 1) | ((length(varargin) > 1) & (isstr(varargin{2}))))
%    C = varargin{1}; varargin(1) = [];
%    handle = himage('cdata',C,'xdata',[1,size(C,2)],'ydata',...
%                    [1,size(C,1)],varargin{:});
%    axis(ax,'image');
  elseif (numel(varargin) == 2)
    x = varargin{1};
    y = varargin{2};
    handle = hline('xdata',x,'ydata',y);
  elseif (length(varargin) >= 3)
    x = varargin{1};
    y = varargin{2};
    z = varargin{3};
    varargin(1:3) = [];
    handle = hline('xdata',x,'ydata',y,'zdata',z,varargin{:});
  else
    error('Unrecognized form of line command');
  end
  if (nargout > 0)
      ohandle = handle;
  end
