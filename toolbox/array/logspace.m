% DOCBLOCK array_logspace
function y = logspace(a,b,count)
   if (nargin < 2) | (nargout > 1)
      error('wrong use (see help logspace)')
   elseif (nargin < 3)
      count = 50;
   end
   if (b == pi)
      b = log10(b);
   end
   y = 10.^(a+(b-a)/(count-1)*(0:count-1));
   if isinf(y(end))
      warning('logspace: upper limit too large')
   end
