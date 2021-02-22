% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function b = stcmp(source,pattern)
b = zeros(size(source),'logical');
for i=1:numel(source)
  b(i) = strncmp(source{i},pattern,length(source{i}));
end
