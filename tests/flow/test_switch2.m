% Test the switch statement with a string argument and one integer case
% The type mismatch should not present a problem.
function test_val = test_switch2

test_val = 0;
x = 'astring';

switch x
case 30
test_val = 0;
case 'astring';
test_val = 1;
case 'cstring';
test_val = 0;
otherwise
test_val = 0;
end
