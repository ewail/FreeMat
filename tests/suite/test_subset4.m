% Test the vector subset with a struct array
function test_val = test_subset4
a = struct('foo',4,'goo',{1,2,3,4});
c = a(3);
test_val = test((c.foo == 4) & (c.goo == 3));
