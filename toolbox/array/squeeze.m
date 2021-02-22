% DOCBLOCK array_squeeze
% Copyright 2004-2005, 2006 Brian Yanoff, Dirk Beque
% Licensed under the GPL
function B = squeeze(A)
  sz = size(A);
  if (length(sz) == 2) 
    B = A;
    return;
  end
  sz(find(sz==1)) = [];
  if length(sz)==1
    sz = [sz,1];
  end
  if length(sz)==0
    sz = [1,1];
  end
  B = reshape(A,sz);
  

  
