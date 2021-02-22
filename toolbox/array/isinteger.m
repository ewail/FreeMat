% DOCBLOCK array_isinteger

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function x = isinteger(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','uint64','int64'},class(y)));
