% test calls with pass-by-reference
function test_val = test_call3
d1 = 3;
test_call3_assist(d1);
test_val = test(d1 == 1);

function test_call3_assist(&x)
x = 1;
