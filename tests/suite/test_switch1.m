% Test the switch statement with a string argument
function test_val = test_switch1

test_val = 0;
x = 'astring';

switch x
case 'bstring'
test_val = 0;
case 'astring';
test_val = 1;
case 'cstring';
test_val = 0;
otherwise
test_val = 0;
end
