function x = testeq(a,b)
  if (size(a,1) ~= size(b,1) || size(a,2) ~= size(b,2))
    x = 0;
    return;
  end
  d = full(a)-full(b);
  if (strcmp(typeof(d),'double') | strcmp(typeof(d),'dcomplex'))
    x = isempty(find(abs(d)>10*eps));
  else
    x = isempty(find(abs(d)>10*feps));
  end
