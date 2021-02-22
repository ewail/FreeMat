% Check the size function with two arguments and one output
function test_val = test_size3
a = [1,2,3;4,5,6];
n = size(a,2);
test_val = test(n == 3);
