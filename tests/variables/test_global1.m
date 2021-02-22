function test_val = test_global1
accum = 0;
for (i=1:10)
  accum = accum + test_global1_assist;
end
test_val = test(accum == 55);

function tick = test_global1_assist
global count_test_global1_assist
count_test_global1_assist = count_test_global1_assist + 1;
tick = count_test_global1_assist;
