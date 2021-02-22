function y = subsref(a,s)
switch s.type
 case '.'
  ind = s.subs;
  y = ind;
 otherwise
  error('specify value for x as p.x(foo)');
end
