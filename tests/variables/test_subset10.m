% Test the cell vector subset with contents-based addressing
function test_val = test_subset10
  a = {5,8;10,3};
  sm = test_subset10_assist(a{2,1:2});
  test_val = test(sm == 13);

function a = test_subset10_assist(b,c)
  a = b + c;
