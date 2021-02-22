% DOCBLOCK handle_view

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function view(varargin)
  if (nargin == 0)
    error('Expected an argument to view function');
  end;
  if (nargin == 1)
    arg = varargin{1};
    if (length(arg) == 1)
      if (arg == 2)
	view(0,90);
	return;
      elseif (arg == 3)
	view(37.5,30);
	return;
      else
	error('Unrecognized form of view');
      end
    end
  else
    az = varargin{1};
    el = varargin{2};
  end
  az = az*pi/180;
  if el == 0
     el = .001;
  end
  el = el*pi/180;
  % The view point is first calculated by transforming to
  % spherical coordinates
  % Calculate the radius of the cube
  xlim = get(gca,'xlim'); xmean = mean(xlim);
  ylim = get(gca,'ylim'); ymean = mean(ylim);
  zlim = get(gca,'zlim'); zmean = mean(zlim);
  xmax = max(abs(xlim-xmean));
  ymax = max(abs(ylim-ymean));
  zmax = max(abs(zlim-zmean));
  r = sqrt(xmax^2+ymax^2+zmax^2);
  z = sin(el)*r + zmean;
  y = -cos(el)*cos(az)*r + ymean;
  x = cos(el)*sin(az)*r + xmean;
  set(gca,'cameraposition',[x,y,z]);
  if (abs(el-pi/2) < .001)
    set(gca,'cameraupvector',[0,1,0]);
  else
    set(gca,'cameraupvector',[0,0,1]);
  end
