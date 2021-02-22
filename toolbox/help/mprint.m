% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function mprint(namebase)
  drawnow
  print(sprintf('%s.jpg',namebase));
  print(sprintf('%s.png',namebase));
