% Test the ndim subset with a cell-array vector
function test_val = test_subset7
a = {1,2,3;4,5,6;7,8,9};
b = a(2,2:3);
test_val = test(strcmp(typeof(b),'cell')) & test((b{1} == 5) & (b{2} == 6));
