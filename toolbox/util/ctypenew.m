% DOCBLOCK external_ctypenew

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function a = ctypenew(typename,count)
  if (nargin == 1)
    count = 1;
  end
  if (nargin == 0)
    error('ctypenew requires a typename argument');
  end
  a = ctypethaw(zeros(1,ctypesize(typename,count),'uint8'),typename,count);
