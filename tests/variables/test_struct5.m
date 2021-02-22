% Test the field dereference as an expression list
function test_val = test_struct5
  a = struct('foo',{5,8,10});
  sm = test_struct5_assist(a.foo);
  test_val = test(sm == 23);

function a = test_struct5_assist(b,c,d)
  a = b + c + d;
