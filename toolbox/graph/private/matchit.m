% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function b = matchit(t,dictionary)
  b = any(stcmp(dictionary,t));
