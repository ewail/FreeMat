0
a = threadnew;
global count                   % register the global variable count
count = 0;
threadstart(a,'freecount',0)   % start the thread
count                          % it is counting
sleep(1)                       % Wait a bit
count                          % it is still counting
threadkill(a)                  % kill the counter
threadwait(a,1000)             % wait for it to finish
count                          % The count will no longer increase
sleep(1)
count
threadfree(a)
