% DOCBLOCK handle_hold

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function h = hold(varargin)
if (nargin == 0)
     hold_toggle(gca);
elseif (strcmp(varargin{1},'on'))
     hold_on(gca);
elseif (strcmp(varargin{1},'off'))
     hold_off(gca);
elseif (ishandle(varargin{1}))
   if (nargin == 1)
     hold_toggle(varargin{1})
   elseif (strcmp(varargin{2},'on'))
     hold_on(varargin{1});
   elseif (strcmp(varargin{2},'off'))
     hold_off(varargin{1});
   else
     error('Unrecognized form of hold');
   end
else
   error('Unrecognized arguments to hold');
end

function hold_off(handle)
  set(gca,'nextplot','replace');

function hold_on(handle)
  set(gcf,'nextplot','add');
  set(gca,'nextplot','add');

function hold_toggle(handle);
  if (strcmp(get(gca,'nextplot'),'replace'))
    set(gca,'nextplot','add');
  else
    set(gca,'nextplot','replace');
  end

