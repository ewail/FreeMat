
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function p = bbtestwriter(a)
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
    p.empty = 1;
    p.filename = '';
    p = class(p,'bbtestwriter');
  elseif isa(a,'bbtestwriter');
    p = a;
  else
    error('copy constructor not defined for bbtestwriter');
  end
    
