% Test the switch statement with a vector argument (should be an error)
function test_val = test_switch4

x = [1 1];
test_val = 0;
try
switch x
case 1
test_val = 0;
otherwise
test_val = 0;
end
catch
test_val = 1;
end
