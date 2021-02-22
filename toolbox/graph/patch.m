% DOCBLOCK handle_patch

% Copyright (c) 2002-2008 Samit Basu, Eugene Ingerman
% Licensed under the GPL

function ohandle = patch(varargin)

   if( nargin==1 )
	error('not implemented')
   end
	
  % search for the propertyname/value pairs
  propstart = 0;
  if (nargin > 2)
    propstart = nargin-1;
    while ((propstart >= 1) && isa(varargin{propstart},'char') && ...
    pvalid('patch',varargin{propstart}))
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
	h = hpatch(propset{:});
  elseif (length(varargin) == 3)
    [vertices, faces, facevertexcdata]=parse_input( varargin{1}, varargin{2}, ones(size(varargin{1})), varargin{3} );
    h = hpatch('xdata',varargin{1},'ydata',varargin{2},'zdata',ones(size(varargin{1})),'cdata',varargin{3},...
	'faces',faces,'vertices',vertices,'facevertexcdata',facevertexcdata,'facecolor','flat','edgecolor','flat',propset{:});
  elseif (length(varargin) == 4)
   [vertices, faces, facevertexcdata]=parse_input( varargin{1}, varargin{2}, varargin{3}, varargin{4} );
    h = hpatch('xdata',varargin{1},'ydata',varargin{2},'zdata',varargin{3},'cdata',varargin{4},...
	'faces',faces,'vertices',vertices,'facevertexcdata',facevertexcdata,'facecolor','flat','edgecolor','flat',propset{:});
  else
    error('Unrecognized arguments to patch command');
  end

if (nargout > 0)
    ohandle = h;
end

function [vertices, faces, facevertexcdata]=parse_input( xdata, ydata, zdata, cdata )
	if( size(xdata) ~= size(ydata) | size(xdata) ~= size(zdata) | length(size(xdata))>2 )
		error('Data size mismatch');
	end
	[nx ny]=size(xdata);
	sz = size(cdata);
	if( ~( (sz(1)==1) | (sz(1)==1 & sz(2)==nx) | (sz(1)==ny & sz(2)==nx) ) )
		error('CData size mismatch');
	end
	vertices=[xdata(:) ydata(:) zdata(:)];
    if nx == 1 
        faces = reshape( 1:nx*ny, [ny nx] )';
    else
        faces = reshape( 1:nx*ny, size(xdata) )';
    end
	facevertexcdata = cdata; %cdata(:);
