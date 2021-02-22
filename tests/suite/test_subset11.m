% Test the vector subset assign function
function test_val = test_subset11
a = [1,2,3;4,5,6];
a(3:4) = [8,9];
test_val = test(a == [1,8,3;4,9,6]);
