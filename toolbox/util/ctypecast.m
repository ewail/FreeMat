% DOCBLOCK external_ctypecast

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function s = ctypecast(s,typename)
  if (nargin ~= 2) error('ctypecast requires a struct and a typename'); end
  s = ctypethaw(ctypefreeze(s,typename),typename);
