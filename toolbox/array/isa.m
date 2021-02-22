% DOCBLOCK inspection_isa

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = isa(x,type)
  if (~strcmp(typeof(type),'char'))
    error('type argument to isa must be a string');
  end
  y = strcmp(class(x),type);
