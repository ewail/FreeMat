0
a = 2; b = 4;    % define a and b (slope and intercept)
y = @(x) a*x+b   % create the anonymous function
y(2)             % evaluate it for x = 2
a = 5; b = 7;    % change a and b
y(2)             % the value did not change!  because a=2,b=4 are captured in y
y = @(x) a*x+b   % recreate the function
y(2)             % now the new values are used
