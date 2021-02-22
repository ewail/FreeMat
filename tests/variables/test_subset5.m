% Test the vector subset with cell array without contents-based addressing
function test_val = test_subset5
a = {1,2,3,4;5,6,7,8};
b = a(2);
test_val = test(strcmp(typeof(b),'cell')) & test(b{1} == 5);
