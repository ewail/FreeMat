% Test the ability to combine structues with different fields - when valid
function test_val = test_struct2
a.foo = 1;
a.goo = 2;
b.goo = 5;
c = [a,b];
test_val = (c(1).foo == 1) & (c(1).goo == 2) & (c(2).goo == 5);
