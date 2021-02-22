% DOCBLOCK freemat_filesep

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = filesep
if (strcmp(computer,'PCWIN'))
  x = '\';
else
  x = '/';
end
