% DOCBLOCK handle_colorbar

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function handle = colorbar(varargin)
axhan = gca;
% Check for an existing colorbar for the current axis
cba = findColorBar(axhan);
if (isempty(cba))
  %Add a new one
  % Resize axhan
  pos = get(axhan,'outerposition');
  width = 0.3;
  pos(3) = pos(3) - width;
  set(gca,'outerposition',pos);
  npos = [pos(1)+pos(3),pos(2),width,pos(4)];
  cba = axes('outerposition',npos,'yaxislocation','right','xtick','none','tag','colorbar','userdata',axhan);
end
if (nargin > 0)
  axes(cba);
  set(cba,varargin{:});
end
cmap = get(gcf,'colormap'); N = size(cmap,1);
cmap = linspace(0,N,N)';
cmap = repmat(cmap,[1,4]);
han = himage('ydata',get(axhan,'clim'),'cdata',cmap);
axis('tight');
axes(axhan);

function cba = findColorBar(axhan)
fig = get(axhan,'parent');
peers = get(fig,'children');
for i = 1:numel(peers)
  if (ishandle(peers(i),'axes') && (strcmp(get(peers(i),'tag'),'colorbar')) && (get(peers(i),'userdata') == axhan))
    cba = peers(i);
    return;
  end
end
cba = [];



