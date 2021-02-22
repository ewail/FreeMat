% DOCBLOCK curvefit_polyint
function pint = polyint(p,k)
   if nargin < 1
      error('wrong use (see help polyint)');
   elseif nargin < 2
      k = 0;
   end
   pint = [(p(:).')./(length(p):-1:1),k];
