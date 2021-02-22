% DOCBLOCK handle_clabel

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function handles = clabel(contourhandle,varargin)
  if (~strcmp(get(contourhandle,'type'),'contour'))
    error('First argument to clabel must be the handle of a contour.');
  end
  C = get(contourhandle,'contourmatrix');
  vals_to_label = get(contourhandle,'levellist');
  if ((numel(varargin) > 0) && (~isstr(varargin{1})))
    vals_to_label = varargin{1};
    varargin = varargin(2:end);
  end
  % Loop through the contours
    colptr = 1;
    while (colptr < size(C,2))
      % Get the next line segment
      linestart = colptr+1;
      linestop = C(2,colptr) + linestart - 1;
      lineval = C(1,colptr);
      if (any(close_enough(vals_to_label-lineval)))
        xvals = C(1,linestart:linestop);
        yvals = C(2,linestart:linestop);
        % Compute the svec - the distance along the line
        dxvals = xvals(2:end)-xvals(1:end-1);
        dyvals = yvals(2:end)-yvals(1:end-1);
        angs = atan2(dyvals,dxvals)*180/pi;
        svals = sqrt(dxvals.^2 + dyvals.^2);
        sdist = cumsum(svals);
        sndx = 10;
        hlist = [];
        while (sndx < numel(sdist)-10) 
          h = text(xvals(sndx),yvals(sndx),sprintf('%g',lineval), ...
                   'rotation',angs(sndx),varargin{:});
          hlist = [hlist,h];
          [a,sndx] = find(sdist > sdist(sndx)+max(sdist)/3,1);
        end
      end
      colptr = linestop + 1;
    end
    if (nargout > 0)
      handles = hlist;
    end

function u = close_enough(d)
  u = abs(d) < 100*eps;
  
