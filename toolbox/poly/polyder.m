% DOCBLOCK curvefit_polyder
function [pder1,pder2] = polyder(p1,p2)
   if nargin < 1 | nargout > nargin
      error('wrong use (see help polyder)');
   end
   if (nargin == 1) 
      % Simple derivative case
      n = numel(p1);
      if (n <= 1)
        pder1 = 0;
        return;
      end;
      x1 = (p1(:).').*(n-1:-1:0);
      x1 = x1(1:end-1);
      pder1 = polyder_trim_zeros(x1);
      if (isa(p1,'single'))
        pder1 = single(pder1);
      end
      return;
   end
   f1 = conv(p1,polyder(p2));
   f2 = conv(polyder(p1),p2);
   m = max(numel(f1),numel(f2));
   f1 = [zeros(1,m-numel(f1)),f1(:).'];
   f2 = [zeros(1,m-numel(f2)),f2(:).'];
   if (nargout < 2)
     pder1 = polyder_trim_zeros(f1+f2);
   else
     pder1 = polyder_trim_zeros(f1-f2);
     pder2 = polyder_trim_zeros(conv(p2,p2));
   end;
   if (isa(p1,'single'))
     pder1 = single(pder1);
   end
   
function y = polyder_trim_zeros(x)
  if (isempty(x) | isempty(find(x,1)))
    y = 0;
  else
    y = x(find(x,1):end);
  end

