% Test the switch statement with a string argument and a cell array
function test_val = test_switch6

test_val = 0;
x = 'astring';

switch x
case 'bstring'
test_val = 0;
case {'dstring',5,'astring'};
test_val = 1;
case 'cstring';
test_val = 0;
otherwise
test_val = 0;
end
