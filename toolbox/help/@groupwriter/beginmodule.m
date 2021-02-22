
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function beginmodule(&p,sourcepath,modname,moddesc,secname,section_descriptors)
  for i = 1:numel(p.clients)
    beginmodule(p.clients{i},sourcepath,modname,moddesc,secname,section_descriptors);
  end
