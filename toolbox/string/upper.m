% DOCBLOCK string_upper

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = upper(x)
  if (isstr(x))
    y = upper_string(x);
  elseif (iscell(x))
    y = cell(size(x));
    for i=1:numel(x)
      y{i} = upper_string(x{i});
    end
  else
    y = x;
  end

function y = upper_string(x)
  if (isstr(x))
    y = string(x + ('A'-'a')*(x>='a' && x<='z'));
  else
    y = x;
  end;
