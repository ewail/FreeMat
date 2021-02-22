% Test the while statement for normal operation
function test_val = test_while1
sum = 0;
i = 1;
while (i<=10)
  sum = sum + i;
  i = i + 1;
end
test_val = test(sum == 55);
