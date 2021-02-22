function p = foo(a)
  if (nargin == 0)
    p.c = [];
    p.id = {1 2 3 4 5 6 7 8 9};
    p = class(p,'foo');
  elseif (isa(a,'foo'));
    p = a;
  else
    p.c = a;
    p.id = {1 2 3 4 5 6 7 8 9};
    p = class(p,'foo');
  end
  