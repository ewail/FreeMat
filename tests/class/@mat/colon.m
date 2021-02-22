function c = colon(a,b,d)
  if (nargin == 2)
    a = mat(a);
    b = mat(b);
    c = mat(a.c : b.c);
  else
    a = mat(a);
    b = mat(b);
    d = mat(d);
    c = mat(a.c : b.c : d.c);
  end


