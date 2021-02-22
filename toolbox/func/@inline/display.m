% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function display(obj)
  printf('  inline function object\n');
  printf('  f(');
  for i=1:numel(obj.args)
    printf('%s',obj.args{i});
    if (i < numel(obj.args))
      printf(',');
    end
  end
  printf(') = %s\n',obj.expr);
