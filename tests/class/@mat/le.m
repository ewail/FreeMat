function c = le(a,b)
  a = mat(a);
  b = mat(b);
  c = mat(a.c <= b.c);
