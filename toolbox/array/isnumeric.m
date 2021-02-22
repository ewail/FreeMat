% DOCBLOCK inspection_isnumeric

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = isnumeric(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','uint64','int64','single','double'},class(y)));
