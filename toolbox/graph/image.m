% DOCBLOCK handle_image

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function ohandle = image(varargin)
  ax = newplot;
  if (length(varargin) == 0), return; end
  % check for formal syntax
  if (isstr(varargin{1}))
    handle = himage(varargin{:});
  elseif ((length(varargin) == 1) || ((length(varargin) > 1) && (isstr(varargin{2}))))
    C = varargin{1}; varargin(1) = [];
    cols = max(2,size(C,2));
    rows = max(2,size(C,1));
    handle = himage('cdata',C,'xdata',[1,cols],'ydata',[1,rows],varargin{:});
    axis(ax,'image');
  elseif (numel(varargin) == 2)
    C = varargin{1};
    cols = max(2,size(C,2));
    rows = max(2,size(C,1));
    handle = himage('cdata',C,'xdata',[1,cols],'ydata',[1,rows]);
    axis maximal;
    axis tight;
    zoom(varargin{2});
  elseif (length(varargin) >= 3)
    x = varargin{1};
    y = varargin{2};
    C = varargin{3};
    varargin(1:3) = [];
    handle = himage('xdata',x([1,end]),'ydata',y([1,end]),'cdata',C,varargin{:});
%    axis(ax,'image');
  else
    error('Unrecognized form of image command');
  end
  if (nargout > 0)
      ohandle = handle;
  end
