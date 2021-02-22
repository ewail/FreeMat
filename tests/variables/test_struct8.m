% Test the structure field-based multiple assign in a function call
function test_val = test_struct8
a = struct('foo',{0,0,0,0});
[a.foo] = test_struct8_assist;
test_val = test(a(1).foo == 1) & test(a(2).foo == 2) & ...
    test(a(3).foo == 3) & test(a(4).foo == 4);

function [a,b,c,d] = test_struct8_assist
  a = 1;
  b = 2;
  c = 3;
  d = 4;
