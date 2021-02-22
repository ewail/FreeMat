function freecount
  global count
  if (~exist('count')) count = 0; end  % Initialize the counter
  while (1)
    count = count + 1;                 % Update the counter
  end
