function y = subsasgn(a,s,b)
switch s.type
case '()'
  ind = s.subs{:};
  a.c(ind) = b;
  y = mat(a.c);
otherwise
  error('Specify value for x as p(x)');
end
