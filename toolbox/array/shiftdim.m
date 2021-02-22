% DOCBLOCK array_shiftdim

% Copyright (c) 2007 Samit Basu
% Licensed under the GPL

function [y,n] = shiftdim(x,p)
  % Compute the size of x
    xsize = size(x);
    if (nargin == 1)
      % Find the first non-singular dimension of x
      p = find(xsize ~= 1,1,'first')-1;
    end
    if (isempty(p) || (p==0))
      % Nothing required
      y = x;
      n = 0;
    elseif (p < 0)
      % For right shifting (negative p), we just reshape the array
      % and introduce extra singleton dimensions
      y = reshape(x,[ones(1,-p),xsize]);
      n = p;
    else
      % Left shifting is modulo the number of dimensions of x
      p = rem(p,ndims(x));
      % For left shifting, (positive p), we can just reshape the
      % array, provided that the leading dimensions are all singletons
      % We have to make sure that the reshape call has at least two
      % dimensions in the shape argument - the number remaining
      % is ndims(x)-p
      if (all(xsize(1:p)==1))
        xsize = xsize((p+1):end);
        if (numel(xsize) < 2), xsize = [xsize,1]; end;
        y = reshape(x,xsize);
        n = p;
      else
        % a permutation is in order
        y = permute(x,[(p+1):ndims(x),1:p]);
        n = p;
      end
    end
    
    
      
      
      
