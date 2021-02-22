0
rand('state',0)    % restores us to startup conditions
a = rand(1,3)      % random sequence 1
b = rand('state'); % capture the state vector
c = rand(1,3)      % random sequence 2  
rand('state',b);   % restart the random generator so...
c = rand(1,3)      % we get random sequence 2 again
