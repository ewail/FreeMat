function evenoddtest(n)
  if (n==0)
    error('zero is neither even nor odd');
  elseif ( n ~= fix(n) )
    error('expecting integer argument');
  end;
  if (n==int32(n/2)*2)
    printf('%d is even\n',n);
  else
    printf('%d is odd\n',n);
  end
