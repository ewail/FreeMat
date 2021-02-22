% DOCBLOCK handle_contour3

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = contour3(varargin)
  handle = contour(varargin{:});
  set(handle,'floating','on');
if ~ishold
	view( handle, 3 );
	grid;
end
  if (nargout > 0) 
    ohandle = handle;
  end
  
