% Test the switch statement with a numerical argument and a cell array
function test_val = test_switch7

test_val = 0;
x = 5;

switch x
case 'bstring'
test_val = 0;
case {'dstring',5.0,'astring'};
test_val = 1;
case 'cstring';
test_val = 0;
otherwise
test_val = 0;
end
