% DOCBLOCK handle_plot3

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function hout = plot3(varargin)
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
  h = [];
  while (~isempty(varargin))
    cs = ''; ms = ''; ps = '';
    if (length(varargin) < 3)
      error('plot3 requires triplets of x, y, z coordinates');
    end;
    if (length(varargin) == 3 || (length(varargin) > 3) && ~islinespec(varargin{4},cs,ms,ps))
      h = [h,plot_triplet(varargin{1},varargin{2},varargin{3},handle,propset)];
      varargin(1:3) = [];
    elseif ((length(varargin) >= 4) && islinespec(varargin{4},cs,ms,ps))
      h = [h,plot_triplet(varargin{1},varargin{2},varargin{3},handle,completeprops(cs,ms,ps,propset))];
      varargin(1:4) = [];
    end;
  end
if ~ishold
	view( handle, 3 );
	grid;
end
axes(saveca);
if (nargout > 0)
   hout = h;
end
    
function h = plot_triplet(X,Y,Z,handle,lineprops)
    h = [];
    if ((isvector(X) | isvector(Y) | isvector(Z)) && (~isvector(X) | ~isvector(Y) | ~isvector(Z)))
      rows = max([size(X,1),size(Y,1),size(Z,1)]);
      cols = max([size(X,2),size(Y,2),size(Z,2)]);
      X = expandmat(X,rows,cols);
      Y = expandmat(Y,rows,cols);
      Z = expandmat(Z,rows,cols);
    end
    if (isvector(X)), X = X(:); end;
    if (isvector(Y)), Y = Y(:); end;
    if (isvector(Z)), Z = Z(:); end;
    for i=1:size(Z,2)
      h = [h,tplotvector(handle,X(:,i),Y(:,i),Z(:,i),lineprops)];
    end

function x = expandmat(a,rows,cols)
if (length(b) == rows)
  x = repmat(b(:),[1,cols]);
elseif (length(b) == cols)
  x = repmat(b(:)',[rows,1]);
else
  error('plot3(X,Y,Z) where one or more arguments are vectors requires the other arguments to have a matching dimension');
end

function k = tplotvector(handle,x,y,z,lineprops)
  set(handle,'color','w');
  ndx = length(get(handle,'children'))+1;
  % Get the colororder
  colororder = get(handle,'colororder');
  % select the row using a modulo
  ndxmod = round(mod(ndx-1,size(colororder,1))+1);
  k = hline('xdata',x,'ydata',y,'zdata',z,'color',colororder(ndxmod,:),lineprops{:});

