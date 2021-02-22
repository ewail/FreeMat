% Test the field set function with a scalar argument
function test_val = test_struct6
  a = struct('foo',5,'color','blue');
  a.color = 'red';
  test_val = test(strcmp(a.color,'red'));
