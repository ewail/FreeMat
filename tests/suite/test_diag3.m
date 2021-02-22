% Test the diagonal creation function
function test_val = test_diag3
a = [2,3];
b = diag(a);
test_val = test(b == [2,0;0,3]);
