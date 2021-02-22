% DOCBLOCK inspection_length

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function len = length(x)
if (isempty(x))
  len = 0;
else
  len = max(size(x));
end
  
