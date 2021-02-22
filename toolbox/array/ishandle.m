% DOCBLOCK inspection_ishandle

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function b = ishandle(handle,type)
    if (isstr(handle))
      b = false;
      return;
    end
    if (strcmp(type,'figure') && (handle<=100))
        b = logical(1);
        return;
    end
    if ((handle<=100000) || (handle ~= int32(handle)))
        b = logical(0);
        return;
    end
    b = logical(0);
    try
        b = strcmp(get(handle,'type'),type);
    catch
    end
