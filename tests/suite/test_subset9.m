% Test the cell vector subset with contents-based addressing
function test_val = test_subset9
  a = {5,8;10,3};
  sm = test_subset9_assist(a{1:3});
  test_val = test(sm == 23);

function a = test_subset9_assist(b,c,d)
  a = b + c + d;
