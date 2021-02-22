% Test the determinant calculation (bug 1584651)
function test_val = test_det1
A = [1 2
       3 4];
test_val = (det(A) == -2)
