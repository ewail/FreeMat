% DOCBLOCK handle_grid

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function h = grid(varargin)
if (nargin == 0)
     grid_toggle(gca);
elseif (strcmp(varargin{1},'on'))
     grid_on(gca);
elseif (strcmp(varargin{1},'off'))
     grid_off(gca);
elseif (ishandle(varargin{1}))
   if (nargin == 1)
     grid_toggle(varargin{1})
   elseif (strcmp(varargin{2},'on'))
     grid_on(varargin{1});
   elseif (strcmp(varargin{2},'off'))
     grid_off(varargin{1});
   else
     error('Unrecognized form of grid');
   end
else
   error('Unrecognized arguments to grid');
end


function grid_toggle(handle);
  if (strcmp(get(gca,'xgrid'),'on') | ...
      strcmp(get(gca,'ygrid'),'on') | ...
      strcmp(get(gca,'zgrid'),'on'))
     grid_off(handle);
  else
     grid_on(handle);
  end

function grid_off(handle)
  set(gca,'xgrid','off');
  set(gca,'ygrid','off');
  set(gca,'zgrid','off');

function grid_on(handle)
  set(gca,'xgrid','on');
  set(gca,'ygrid','on');
  set(gca,'zgrid','on');

