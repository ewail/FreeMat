% Test the ability to add a new field name to a structure
function test_val = test_struct3
a.foo = 1;
a.goo = 2;
c(1) = a;
c(2) = a;
c(1).hoo = 6;
test_val = (c(1).foo == 1) & (c(2).foo == 1) & (c(1).goo == 2) & (c(2).goo == 2) & (c(1).hoo == 6);
