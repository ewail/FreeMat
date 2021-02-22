% DOCBLOCK handle_legend

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function handle = legend(varargin)
  axhan = gca;
  % Check the form of the command
  if (nargin == 1 && strcmp(varargin{1},'off')), legend_off(axhan); return; end
  if (nargin == 1 && strcmp(varargin{1},'hide')), legend_hide(axhan); return; end
  if (nargin == 1 && strcmp(varargin{1},'show')), legend_show(axhan); return; end
  if (nargin == 1 && strcmp(varargin{1},'toggle')), legend_toggle(axhan); return; end
  if (nargin == 1 && strcmp(varargin{1},'boxon')), legend_boxon(axhan); return; end
  if (nargin == 1 && strcmp(varargin{1},'boxoff')), legend_boxoff(axhan); return; end
  % Check for a location specifier
  legend_position = 'ne';
  if (nargin >= 2 && strcmp(varargin{nargin-1},'location'))
    legend_position = parse_location_spec(varargin{nargin});
    varargin((nargin-1):nargin) = [];
    nargin = nargin - 2;	
  end;
  % Check the remaining arguments
  for (i=1:nargin)
    if (~isstr(varargin{i}))
      error('arguments to legend should be strings'); 
    end;
  end
  if (nargin == 0)
    cba = findLegend(axhan);
    if (isempty(cba))
      children = get(axhan,'children');
      for i=1:numel(children)
        varargin{i} = sprintf('data%d',i);
      end
    else
      p = get(cba,'userdata');
      varargin = p.args;
    end
    nargin = numel(varargin);
  end
  legend_off(axhan);
  % Get the current axis inner bounds
  set(axhan,'tag',get(axhan,'tag'));
  pos = get(axhan,'position');
  % Convert to pixels
  figsize = get(get(axhan,'parent'),'figsize');
  pos = pos.*[figsize,figsize];
  % Get the height of a text line
  textheight = get(axhan,'textheight');
  % We will pad by textheight on all four sides
  children = get(axhan,'children');
  entryCount = min(numel(get(axhan,'children')),nargin);
  padfact = 1.1;
  legrows = (entryCount+2)*textheight*padfact;
  maxlen = 0;
  for (i=1:nargin)
    maxlen = max(numel(varargin{i}),maxlen);
  end
  % Create the axes first
  udat.handle = axhan;
  udat.args = varargin;
  cba = axes('outerposition',[0,0,0.01,0.01],'xtick',[],'ytick',[],'xticklabel','','yticklabel','','zticklabel','','xlim',[0,1],'ylim',[0,1],'tag','legend','userdata',udat);
  % Add the text labels to the axis
  texthandles = [];
  maxwidth = 0;
  maxheight = 0;
  for (i=1:entryCount)
    if (ishandle(children(i),'line'))
       texthandles(i) = htext('position',[0,0,0],...
	    'fontname',get(axhan,'fontname'),...
	    'fontsize',get(axhan,'fontsize'),...
	    'fontangle',get(axhan,'fontangle'),...
	    'fontweight',get(axhan,'fontweight'),...
	    'string',varargin{i},'verticalalignment','middle');
       bbox = get(texthandles(i),'boundingbox');
       maxwidth = max(maxwidth,bbox(3));
       maxheight = max(maxheight,bbox(4));
    end
  end;
  legcols = maxwidth + 6*textheight;
  % Legend is arranged as
  legpos = [0,0,legcols/figsize(1),legrows/figsize(2)];
  nunx = textheight/legcols;
  nuny = textheight/legrows*padfact;
  % P-S-PLABELP
  % Add an new one
  set(cba,'outerposition',legpos);
  % Add the labels and the symbols to this axis
  for (i=1:entryCount)
    if (ishandle(children(i),'line'))
      hcolor = get(children(i),'color');
      hstyle = get(children(i),'linestyle');
      ypos = 1 - (nuny + (i-1)*nuny + 0.5*nuny);
      line('xdata',[1,4]*nunx,'ydata',[ypos,ypos],...
	    'color',hcolor,'linestyle',hstyle);
      line('xdata',2.5*nunx,'ydata',ypos,...
	    'marker',get(children(i),'marker'),...
	    'markeredgecolor',get(children(i),'markeredgecolor'),...
	    'markerfacecolor',get(children(i),'markerfacecolor'),...
	    'markersize',get(children(i),'markersize'));
      set(texthandles(i),'position',[5*nunx,ypos,0]);
    end
  end
  pbox = get(axhan,'position');
  cx = pbox(1)+pbox(3)/2;
  ty = pbox(2)+pbox(4);
  by = pbox(2);
  cy = pbox(2)+pbox(4)/2;
  lx = pbox(1);
  rx = pbox(1)+pbox(3);
  lw = legpos(3);
  lh = legpos(4);
  switch(legend_position)
    case 'n'
      set(cba,'outerposition',[cx-lw/2,ty-lh,lw,lh]);
    case 's'
      set(cba,'outerposition',[cx-lw/2,by,lw,lh]);
    case 'e'
      set(cba,'outerposition',[rx-lw,cy-lh/2,lw,lh]);
    case 'w'
      set(cba,'outerposition',[lx,cy-lh/2,lw,lh]);
    case 'ne'
      set(cba,'outerposition',[rx-lw,ty-lh,lw,lh]);
    case 'se'
      set(cba,'outerposition',[rx-lw,by,lw,lh]);
    case 'sw'
      set(cba,'outerposition',[lx,by,lw,lh]);
    case 'nw'
      set(cba,'outerposition',[lx,ty-lh,lw,lh]);
    otherwise
      error('Unrecognized position tag for legend');
  end
  axes(axhan);

function legend_boxon(axhan)
  cba = findLegend(axhan);
  if (~isempty(cba))
    set(cba,'visible','on');
  end

function legend_boxoff(axhan)
  cba = findLegend(axhan);
  if (~isempty(cba))
    set(cba,'visible','off');
  end

function legend_toggle(axhan)
  cba = findLegend(axhan);
  if (~isempty(cba))
    if (strcmp(get(cba,'visible'),'on'))
      legend_hide(axhan);
    else
      legend_show(axhan);
    end
  end

function legend_hide(axhan)
  cba = findLegend(axhan);
  if (~isempty(cba))
    recursive_hide(cba);
  end

function legend_show(axhan)
  cba = findLegend(axhan);
  if (~isempty(cba))
    recursive_show(cba);
  end

function recursive_hide(han)
  if (~isempty(han))
    set(han,'visible','off');
    children = get(han,'children');
    for (i=children)
      recursive_hide(i);
    end
  end

function recursive_show(han)
  if (~isempty(han))
    set(han,'visible','on');
    children = get(han,'children');
    for (i=children)
      recursive_show(i);
    end
  end

function legend_off(axhan)
  cba = findLegend(axhan);
  if (~isempty(cba))
    fig = get(axhan,'parent');
    peers = get(fig,'children');
    peers(peers==cba) = [];
    set(fig,'children',peers);
  end

function cba = findLegend(axhan)
fig = get(axhan,'parent');
peers = get(fig,'children');
for i = 1:numel(peers)
  if (ishandle(peers(i),'axes') && (strcmp(get(peers(i),'tag'),'legend')))
    udat = get(peers(i),'userdata');
    if (udat.handle == axhan)
      cba = peers(i);
      return;
    end
  end
end
cba = [];

function spec = parse_location_spec(t)
  if (~isstr(t))
    error('Unrecognized location specifier');
  end
  switch(t)
    case {'ne','northeast'}
      spec = 'ne';
    case {'se','southeast'}
      spec = 'se';
    case {'nw','northwest'}
      spec = 'nw';
    case {'sw','southwest'}
      spec = 'sw';
    case {'n','north'}
      spec = 'n';
    case {'e','east'}
      spec = 'e';
    case {'w','west'}
      spec = 'w';
    case {'s','south'}
      spec = 's';
    otherwise
      error('Unrecognized location specifier');
 end
  
