% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function b = islinespec(t,&colorspec,&markerspec,&linespec)
% try to parse a string out as a linespec
% a linespec consists of three parts:
%   a colorspec - y,m,c,r,g,b,w,k
%   a markerspec - +o*.xs,square,s,diamond,d,^v><
%   a linespec - -,--,:,-.
if (~isstr(t))
  b = 0;
  return;
end
giveup = 0;
colorspec = 'none';
markerspec = 'none';
linespec = 'none';
orig_t = t;
while (~giveup && length(t)>0)
  giveup = 1;
  if (matchit(t,colorset))
    colorspec = parseit(t,colorset);
    giveup = 0;
  end;
  if (matchit(t,markerset))
    markerspec = parseit(t,markerset);
    giveup = 0;
  end
  if (matchit(t,styleset))
    linespec = parseit(t,styleset);
    giveup = 0;
  end
end
if (giveup)
  b = 0;
else
  b = 1;
end
if (strcmp(markerspec,'none') && strcmp(linespec,'none'))
  linespec = '-';
end
