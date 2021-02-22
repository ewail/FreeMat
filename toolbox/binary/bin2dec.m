% DOCBLOCK typecast_bin2dec

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function t = bin2dec(x)
  t = bin2int(x == '1');
