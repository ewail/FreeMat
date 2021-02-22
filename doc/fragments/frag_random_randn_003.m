0
randn('state',0)    % restores us to startup conditions
a = randn(1,3)      % random sequence 1
b = randn('state'); % capture the state vector
c = randn(1,3)      % random sequence 2  
randn('state',b);   % restart the random generator so...
c = randn(1,3)      % we get random sequence 2 again
