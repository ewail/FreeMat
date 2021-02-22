% DOCBLOCK freemat_pathsep

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = pathsep
if (strcmp(computer,'PCWIN'))
  x = ';';
else
  x = ':';
end
