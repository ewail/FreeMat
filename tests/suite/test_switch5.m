% Test the switch statement with a numerical argument
function test_val = test_switch5

x = 1.2f;
test_val = 0;
switch x
case 1
test_val = 0;
case 1.1f
test_val = 0;
case 1.2+i;
test_val = 0;
case 1.2;
test_val = 0;
case 1.2f;
test_val = 1;
end
