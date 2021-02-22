% Check that test function throws an error when not given an argument
function test_val = test_test5

test_val = 0;

try
  test;
catch
  test_val = 1;
end
