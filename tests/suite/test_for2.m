% Test a simple 'for' statement with a continue clause
function test_val = test_for2
sum = 0;
for i = 1:10;
 sum = sum + i;
 if (i==5), continue; end
 sum = sum - 1;
end
test_val = test(sum == 46);
