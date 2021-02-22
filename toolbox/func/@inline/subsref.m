% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL
function B = subsref(A,S)
  if (length(S) ~= 1)
    error('inline expressions only support () indexing expressions')
  end
  Sref = S(1);
  if (~strcmp(Sref.type,'()'))
    error('unsupported operation on inline object');
  end
  t = struct(A);
  B = inline_evaluate(t.expr,t.assign_args,Sref.subs);

