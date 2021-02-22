function c = power(a,b)
  a = mat(a);
  b = mat(b);
  c = mat(a.c .^ b.c);
