% Test the ability to combine structures with different ordering in their elements.
function test_val = test_struct1
a.foo = 1;
a.goo = 2;
b.goo = 5;
b.foo = 4;
c = [a,b];
test_val = (c(1).foo == 1) & (c(2).foo == 4) & (c(1).goo == 2) & (c(2).foo == 4);
