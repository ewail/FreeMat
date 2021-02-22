
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function p = textwriter(a)
  if (nargin == 0)
    p.myfile = -1;
    p.sectables = {};
    p.eqnlist = {};
    p.verbatim = false;
    p.modulename = '';
    p.groupname = '';
    p.ignore = false;
    p.section_descriptors = [];
    p.sourcepath = '';
    p = class(p,'textwriter');
  elseif isa(a,'textwriter');
    p = a;
  else
    error('copy constructor not defined for textwriter');
  end
    
