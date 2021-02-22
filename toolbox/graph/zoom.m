% DOCBLOCK handle_zoom

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function zoom(factor)
% Get the current axis handle
ax = gca;
% It should contain an image object
imhan = get_image_child(ax);
if (isempty(imhan)) return; end;
% Get the size of the image objects Cdata
C = get(imhan,'cdata');
newsize = size(C);
if (factor > 0)
  newsize = newsize * factor;
elseif (factor == 0)
  axis image;
  return;
elseif (factor < 0)
  axis normal;
  return;
end
% Get the axis position vector
position = get(ax,'position');
new_width = newsize(2)/position(3);
new_height = newsize(1)/position(4);
sizefig(new_width,new_height);

function handle = get_image_child(ax)
children = get(ax,'children');
handle = [];
for i=1:numel(children)
  if (ishandle(children(i),'image'))
    handle = children(i);
    return;
  end
end

