function c = keyfunc(a,b,operation,printit)
  if (~isset('a') | ~isset('b'))
    error('keyfunc requires at least the first two 2 arguments');
  end;
  if (~isset('operation'))
    % user did not define the operation, default to '+'
    operation = '+';
  end
  if (~isset('printit'))
    % user did not specify the printit flag, default is false
    printit = 0;
  end
  % simple operation...
  eval(['c = a ' operation ' b;']);
  if (printit)
    printf('%f %s %f = %f\n',a,operation,b,c);
  end
