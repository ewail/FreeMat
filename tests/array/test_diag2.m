% Test the diagonal extraction function with a non-zero diagonal
function test_val = test_diag2
a = [1,2,3,4;5,6,7,8;9,10,11,12];
b = diag(a,1);
test_val = test(b == [2;7;12]);
