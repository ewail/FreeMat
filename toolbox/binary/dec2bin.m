% DOCBLOCK typecast_dec2bin

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function t = dec2bin(x,n)
  x = x(:); 
  if (size(x) == [0,0])
      t = string([]);
      return;
  end
  if (~exist('n') && max(x) > 0)
    n = ceil(log2(max(x)+1e-10));
  elseif (~exist('n'))
  t = string(zeros(size(x))+'0');
    return;
  elseif (max(x) == 0)
  t = string(zeros(size(x))+'0');
    return;
  end
  n = max(1,n);
  t = string(int2bin(x,n)+'0');
