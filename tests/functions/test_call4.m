% test calls with pass-by-reference
function test_val = test_call4
d1.foo = 3;
test_call4_assist(d1.foo);
test_val = test(d1.foo == 1);

function test_call4_assist(&x)
x = 1;
