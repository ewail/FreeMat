1
a = threadnew
threadstart(a,'error',0,'Hello world!'); % Will immediately stop due to error
c = threadvalue(a)                     % The error comes to us
threadfree(a)
