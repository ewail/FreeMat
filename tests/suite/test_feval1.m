function test_val = test_feval1
y = 0;
test_val = feval('test_feval1_local_func',y);

function z = test_feval1_local_func(x)
z = 1;
