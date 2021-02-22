% Test a simple 'for' statement with a continue clause
function test_val = test_for3
sum = 0;
for i = 1:10;
 sum = sum + i;
 if (i==5), break; end;
end
test_val = test(sum == 15);
