% Test the ndim subset with a regular vector
function test_val = test_subset6
a = [1,2,3;4,5,6;7,8,9];
b = a(1:2,2:3);
c = [2,3;5,6];
test_val = test(b == c);
