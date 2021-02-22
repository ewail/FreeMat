% Test the diagonal extraction function
function test_val = test_diag1
a = [1,2,3,4;5,6,7,8;9,10,11,12];
b = diag(a);
test_val = test(b == [1;6;11]);
