0
a = threadnew;
threadstart(a,'sleep',0,10);  % start a thread that will sleep for 10
threadwait(a,2000)            % 2 second wait is not long enough
threadwait(a,10000)           % 10 second wait is long enough
threadfree(a)
