% DOCBLOCK inspection_isfield

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = isfield(x,field)
  if (isa(x,'struct'))
    y = any(strcmp(fieldnames(x),field));
  else
    y = logical(0);
  end
