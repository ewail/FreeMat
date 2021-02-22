% Test the diagonal creation function with a non-zero diagonal
function test_val = test_diag4
a = [2,3];
b = diag(a,-1);
test_val = test(b == [0,0,0;2,0,0;0,3,0]);
