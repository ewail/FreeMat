% DOCBLOCK handle_axis

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function retvec = axis(varargin);
if (length(varargin) > 0)
  if (ishandle(varargin{1},'axes'))
    ax = varargin{1};
    varargin(1) = [];
  else
    ax = gca;
  end
else
  ax = gca;
end
saveca = gca;
axes(ax);
if (length(varargin) == 0)
  % no args, just return the limits
  if (is2dview(ax))
    retvec = [get(ax,'xlim'),get(ax,'ylim')];
  else 
    retvec = [get(ax,'xlim'),get(ax,'ylim'),get(ax,'zlim')];
  end
else
  mode = varargin{1};
  if (isnumeric(mode))
    HandleNumericMode(ax,mode);
  elseif (strncmp(mode,'auto',4))
    HandleAutoMode(ax,mode);
  elseif (strcmp(mode,'manual'))
    HandleManualMode(ax);
  elseif (strcmp(mode,'tight'))
    HandleTightMode(ax);
  elseif (strcmp(mode,'fill'))
    HandleFillMode(ax);
  elseif (strcmp(mode,'ij'))
    HandleIJMode(ax);
  elseif (strcmp(mode,'xy'))
    HandleXYMode(ax);
  elseif (strcmp(mode,'equal'))
    HandleEqualMode(ax);
  elseif (strcmp(mode,'image'))
    HandleImageMode(ax);
  elseif (strcmp(mode,'square'))
    HandleSquareMode(ax);
  elseif (strcmp(mode,'normal'))
    HandleNormalMode(ax);
  elseif (strcmp(mode,'vis3d'))
    HandleVis3DMode(ax);
  elseif (strcmp(mode,'off'))
    HandleOffMode(ax);
  elseif (strcmp(mode,'on'))
    HandleOnMode(ax);
  elseif (strcmp(mode,'maximal'))
    HandleMaximalMode(ax);
  else
    error(['Unrecognized mode ' mode ' argument to axis command']);
  end
end
axes(saveca);

% axis autoxz --> xlimmod->auto, zlimmode->auto, ylimmode->manual
% axis manual --> xyzlimmode->manual
% axis tight  --> set tight limits
% axis fill   --> set plot box aspect so that axis fills position
% axis ij     --> xdir->normal, ydir->reverse
% axis xy     --> xdir->normal, ydir->normal
% axis equal  --> 3D plot, --> image,
%                 2D plot, dar->111, ?
% axis image  --> dar->111, pbar-->auto, tight
% axis square --> pbar->111, dar->auto
% axis normal --> pbar->auto, dar->auto
% axis vis3d  --> darmode->manual, pbarmode->manual
% axis off    --> visible off, title->visible on
% axis on     --> visible on

function HandleFillMode(ax)
  pos = get(ax,'position');
  fsize = get(get(ax,'parent'),'figsize');
  width = pos(3)*fsize(1); height = pos(3)*fsize(2);
  set(ax,'plotboxaspectratio',[width,height,1]);

function HandleOnMode(ax)
  set(ax,'visible','on');

function HandleOffMode(ax)
  set(ax,'visible','off');

function HandleVis3DMode(ax)
  set(ax,'plotboxaspectratiomode','manual','dataaspectratiomode','manual');

function HandleNormalMode(ax)
  set(ax,'plotboxaspectratiomode','auto','dataaspectratiomode','auto');

function HandleSquareMode(ax)
  set(ax,'plotboxaspectratio',[1,1,1],'dataaspectratiomode','auto');

function HandleImageMode(ax)
  set(ax,'dataaspectratio',[1,1,1],'plotboxaspectratiomode','auto');
  lims = get(ax,'datalimits');
  set(ax,'xlim',lims(1:2),'ylim',lims(3:4),'zlim',lims(5:6));
  set(ax,'ydir','reverse');

function HandleEqualMode(ax)
  set(ax,'dataaspectratio',[1,1,1]);

function HandleXYMode(ax)
  set(ax,'xdir','normal','ydir','normal');

function HandleIJMode(ax)
  set(ax,'xdir','normal','ydir','reverse');

function HandleTightMode(ax)
  lims = get(ax,'datalimits');
  set(ax,'xlim',lims(1:2),'ylim',lims(3:4),'zlim',lims(5:6));

function HandleManualMode(ax)
  set(ax,'xlimmode','manual','ylimmode','manual','zlimmode','manual');

function HandleMaximalMode(ax)
  set(ax,'xtick',[],'ytick',[],'ztick',[],'xticklabel','','yticklabel','','zticklabel','');

function HandleAutoMode(ax,mode)
  allactive = strcmp(mode,'auto');
  xactive = any(find(mode == 'x'));
  yactive = any(find(mode == 'y'));
  zactive = any(find(mode == 'z'));
  if (allactive | xactive), set(ax,'xlimmode','auto'), end
  if (allactive | yactive), set(ax,'ylimmode','auto'), end
  if (allactive | zactive), set(ax,'zlimmode','auto'), end

function HandleNumericMode(ax,mode)
  mode = mode(:);
  if (length(mode) == 2)
    set(ax,'xlim',mode);
  elseif (length(mode) == 4)
    set(ax,'xlim',mode(1:2),'ylim',mode(3:4));
  elseif (length(mode) == 6)
    set(ax,'xlim',mode(1:2),'ylim',mode(3:4),'zlim',mode(5:6));
  elseif (length(mode) == 8)
    set(ax,'xlim',mode(1:2),'ylim',mode(3:4),'zlim',mode(5:6),'clim',mode(7:8));
  else
    error('Unrecognized mode in call to axis - expected 2,4,6 or 8 arguments');
  end
