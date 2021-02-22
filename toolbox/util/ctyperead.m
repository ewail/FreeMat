% DOCBLOCK external_ctyperead

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function a = ctyperead(fid,typename,count)
  if (nargin < 3), count = 1; end;
  if (nargin < 2), error('ctyperead requries at least a file id and a typename'); end;
  p = fread(fid,[1,ctypesize(typename,count)],'*uint8');
  a = ctypethaw(p,typename,count);

