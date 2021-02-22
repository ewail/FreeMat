% DOCBLOCK external_ctypewrite

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function ctypewrite(fid,a,typename)
  if (nargin < 3), error('ctypewrite requires 3 args, the fid, variable, and typename'); end;
  fwrite(fid,ctypefreeze(a,typename));

