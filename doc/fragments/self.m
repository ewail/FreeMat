function y = self(n)
  if (n>1)
    y = n + self(n-1);
    printf('y is %d\n',y);
  else
    y = 1;
    printf('y is initialized to one\n');
    keyboard
  end
