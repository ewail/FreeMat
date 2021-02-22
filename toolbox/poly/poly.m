% DOCBLOCK curvefit_poly
function p = poly(r)
   if (nargin < 1) | (nargout > 1)
      error('wrong use (see help poly)')
   end
   
   % Identify arguments
   if issquare(r)
      r = eig(r);
   else
      r = r(:).';
   end
   
   % Strip out infinities
   r = r( ~isinf(r) );
   
   % Expand recursion formula
   n = length(r);
   p = [1,zeros(1,n)];
   for v = 1:n
      p(2:v+1) = p(2:v+1)-r(v)*p(1:v);
   end

   % The result should be real if the roots are complex conjugates.
   r_neg_comp_roots = r(imag(r)<0);
   r_pos_comp_roots = r(imag(r)>0);
   if (numel(r_neg_comp_roots) == numel(r_pos_comp_roots))
     if sort(r(imag(r)>0))-sort(conj(r(imag(r)<0)))<teps(r)
        p = real(p);
     end
   end
