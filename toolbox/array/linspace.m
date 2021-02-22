% DOCBLOCK array_linspace
% Copyright (c) 2002, 2003 Samit Basu
% Licensed under the GPL
function y = linspace(a,b,len)
  if (nargin < 3)
    len = 100;
  end
  if (a == b)
      y = a*ones(1,len);
  else
      y = a + ((0:(len-1))*(b-a)/(len-1));
  end

