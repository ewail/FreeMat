% Test the persistent variable declaration
function test_val = test_persistent1
accum = 0;
for (i=1:10)
  accum = accum + test_persistent1_assist;
end
test_val = test(accum == 55);

function tick = test_persistent1_assist
persistent count;
count = count + 1;
tick = count;
