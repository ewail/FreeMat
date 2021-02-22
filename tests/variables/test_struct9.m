% Test the ability to assign a single element in a structure array
function test_val = test_struct9
d = struct('foo',3,'goo','hello','shoo',1:5);
d(3).shoo = 3;
test_val = test(d(3).shoo == 3);
