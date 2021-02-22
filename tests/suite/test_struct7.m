% Test the field set function with a vector argument
function test_val = test_struct7
  a = struct('foo',{6,5,3},'color','blue');
  test_val = 0;
  try
    a.foo = 7;
  catch
    test_val = 1;
  end
