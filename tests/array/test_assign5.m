% Test the field assignment with a scalar struct
function test_val = test_assign5
a = struct('foo',5);
a.foo = -1;
test_val = test(a.foo == -1);
