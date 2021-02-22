% DOCBLOCK constants_teps

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = teps(x)
switch(typeof(x))
  case {'single'}
    y = feps;
  case {'double','int8','uint8','int16','uint16','int32','uint32','int64','uint64'}
    y = eps;
  otherwise
    error('teps only applies to numerical arrays');
end
