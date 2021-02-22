% DOCBLOCK array_subsref

% Copyright (c) 2007 Samit Basu
% Licensed under the GPL
function b = subsref(a,s)
  for i=1:numel(s)
    switch (s(i).type)
    case '()'
      a = a(s(i).subs{:});
    case '{}'
      a = a{s(i).subs{:}};
    case '.'
      a = a.(s(i).subs);
    otherwise
      error('illegal indexing structure argument to subsref');
    end
  end
  b = a;


