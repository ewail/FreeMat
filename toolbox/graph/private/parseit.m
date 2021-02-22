% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function b = parseit(&t,dictionary)
  n = stcmp(dictionary,t);
  b = dictionary{min(find(n))};
  t(1:length(b)) = [];
