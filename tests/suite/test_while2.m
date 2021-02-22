% Test a simple 'while' statement with a continue clause
function test_val = test_while2
sum = 0;
i = 0;
while (i < 10)
  i = i + 1;
  sum = sum + i;
  if (i==5), continue; end
  sum = sum - 1;
end
test_val = test(sum == 46);
