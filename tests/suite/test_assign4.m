% Test the assignment of a value to a cell array as an n-dim indexing
function test_val = test_assign4
a = {1,2,3;4,5,6};
a{2,3} = -1;
test_val = test(a{6} == -1);
