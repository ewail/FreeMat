% DOCBLOCK transforms_hilbert

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = hilbert(x,n)
  [t,ns] = shiftdim(x);
  if (nargin == 1)
    n = size(t,1);
  end
  f = fft(t,n,1);
  h = zeros(n,1);
  h(1) = 1;
  if (rem(n,2) == 0)
    h((n/2)+1) = 1;
    h(2:(n/2)) = 2;
  else
    h(2:((n+1)/2)) = 2;
  end
  y = ifft(f.*repmat(h,1,size(f,2)));
  y = shiftdim(y,-ns);

