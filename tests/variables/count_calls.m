function count_calls
  persistent ccount
  if isempty(ccount); ccount = 0; end;
  ccount = ccount + 1;
  printf('Function has been called %d times\n',ccount);
