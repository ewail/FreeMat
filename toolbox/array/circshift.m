% DOCBLOCK array_circshift

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function x = circshift(y,shiftvec)
  szey = size(y);
  ndim = prod(size(szey));
  shiftvec = shiftvec(:);
  shiftlen = prod(size(shiftvec));
  d = {};
  for k=1:ndim
    if (k<=shiftlen)
      shift = shiftvec(k);
    else
      shift = 0;
    end
    d{k} = mod((1:szey(k)) - 1 - shift,szey(k)) + 1;
  end;
  x = y(d{:});
