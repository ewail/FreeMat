0
a = threadnew;                         % Create the thread
threadstart(a,'system',1,'ls -lrt /'); % Start the thread
b = rand(100)\rand(100,1);             % Solve some equations simultaneously
c = threadvalue(a);                    % Retrieve the file list
size(c)                                % It is large!
threadfree(a);
