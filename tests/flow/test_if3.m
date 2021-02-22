% Test the if-elseif-else statement
function test_val = test_if3
a = 1;
test_val = 0;
if (a == 0)
  test_val = 0;
elseif (a == 3);
  test_val = 0;
elseif (a == 1);
  test_val = 1;
else
  test_val = 0;
end
