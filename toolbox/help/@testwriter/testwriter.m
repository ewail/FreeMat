
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function p = testwriter(a)
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
    p.recs = {};
    p.num = 1;
    p = class(p,'testwriter');
  elseif isa(a,'testwriter');
    p = a;
  else
    error('copy constructor not defined for testwriter');
  end
    
