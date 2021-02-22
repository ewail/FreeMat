% DOCBLOCK handle_imagesc

% Copyright (c) 2002-2007 Samit Basu, Eugene Ingerman
% Licensed under the GPL

function ohandle = imagesc(varargin)
  	ax = newplot;
	if (length(varargin) == 0), return; end
	lim=[];
	if( (nargin==2) || (nargin==4) )
		if ( (length( varargin{end} ) == 2) && varargin{end}(1)<=varargin{end}(2) ) %last element - limits
			lim=varargin{end};
			varargin{end}=[];
			nargin=nargin-1;
		end
	end
	
	switch(nargin)
		case 1
			handle = image(varargin{1},'cdatamapping','scaled');
		case 3
			handle = image(varargin{:},'cdatamapping','scaled');
		otherwise
			error('Unrecognized form of image command');
	end
	
	if ~isempty(lim),
	  clim(lim);
	end

	if (nargout > 0)
		ohandle = handle;
	end
