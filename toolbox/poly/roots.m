% DOCBLOCK curvefit_roots

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function z = roots(p)
  if(any(isnan(p) | isinf(p)))
     error('Input to ROOTS must not contain NaN or Inf.');
  end
  if (isempty(p))
    z = zeros(0,1,class(p));
    return;
  end
  while(any(isinf(p./p(1))))
    p=p(2:end);
  end
  if (numel(p) <= 1) 
    z = zeros(0,1,class(p));
    return;
  end
  p = vec(p);
  n = numel(p)-1;
  o = ones(n-1,1);
  if (isa(p,'single'))
    o = single(o);
  end
  A = diag(o,-1);
  A(1,:) = -p(2:n+1)./p(1);
  s = eig(A);
  [n,m] = sort(-abs(s));
  z = s(m);
  
