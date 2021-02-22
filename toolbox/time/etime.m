% DOCBLOCK freemat_etime

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = etime(x1,x2)
  if (~exist('x1') | ~exist('x2'))
     error 'etime expects two arguments'
  end
  try
    y1 = clocktotime(x1);
    y2 = clocktotime(x2);
  catch
     error 'invalid input vectors to etime'
  end
  y = y1 - y2;
