% DOCBLOCK handle_pcolor

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ohandle = pcolor(varargin)
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
   else
     handle = newplot;
   end
   saveca = gca;
   axes(handle);
   
   if (nargin == 1)
     C = varargin{1};
     h = surf(1:size(C,2),1:size(C,1),zeros(size(C)),C);
     axis tight;
   elseif (nargin == 3)
     X = varargin{1};
     Y = varargin{2};
     C = varargin{3};
     h = surf(X,Y,zeros(size(C)),C);
     axis tight;
   else
     error('pcolor requires either X,Y,C, or C arguments');
   end
   axes(saveca);
   if (nargout > 0)
     ohandle = h;
   end
