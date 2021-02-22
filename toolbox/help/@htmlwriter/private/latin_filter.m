
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function otext = latin_filter(text)
  otext = strrep(text,'&','&amp;');
  otext = strrep(otext,'<','&lt;');
  otext = strrep(otext,'>','&gt;');
