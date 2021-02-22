% DOCBLOCK string_lower

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = lower(x)
  if (isstr(x))
    y = lower_string(x);
  elseif (iscellstr(x))
    y = cell(size(x));
    for i=1:numel(x)
      y{i} = lower_string(x{i});
    end
  else
    y = x;
  end

function y = lower_string(x)
  y = string(x + ('a'-'A')*(x>='A' && x<='Z'));
