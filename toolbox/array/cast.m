% DOCBLOCK typecast_cast

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cast(a,toclass)
    switch(toclass)
      case 'cell'
        y = cell(a);
      case 'struct'
        y = struct(a);
      case 'logical'
        y = logical(a);
      case 'uint8'
        y = uint8(a);
      case 'int8'
        y = int8(a);
      case 'uint16'
        y = uint16(a);
      case 'int16'
        y = int16(a);
      case 'uint32'
        y = uint32(a);
      case 'int32'
        y = int32(a);
      case 'uint64'
        y = uint64(a);
      case 'int64'
        y = int64(a);
      case {'float','single'}
        y = float(a);
      case 'double'
        y = double(a);
      case 'char'
        y = char(a);
      otherwise
        error(['Unsupported class ' toclass ' requested in typecast'])
    end
    
