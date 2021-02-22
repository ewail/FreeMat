% DOCBLOCK inspection_iscellstr

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function x = iscellstr(y)
  if (~iscell(y))
    x = logical(0);
    return;
  end
  m = numel(y);
  all_strings = logical(1);
  i = 1;
  while (all_strings & (i <= m))
    all_strings = all_strings & (isstr(y{i}) | isempty(y{i}));
    i = i + 1;
  end
  x = all_strings;
