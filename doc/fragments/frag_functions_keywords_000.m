1
keyfunc(1,3)                % specify a and b, defaults for the others
keyfunc(1,3,/printit)       % specify printit is true
keyfunc(/operation='-',2,3) % assigns a=2, b=3
keyfunc(4,/operation='*',/printit) % error as b is unspecified
