% Test a simple 'for' statement
function test_val = test_for1
sum = 0;
for i = 1:10;
 sum = sum + i;
end
test_val = test(sum == 55);
