% Test a simple 'while' statement with a continue clause
function test_val = test_while3
sum = 0;
i = 1;
while (i < 10)
  sum = sum + i;
  if (i==5), break; end
  i = i + 1;
end
test_val = test(sum == 15);
