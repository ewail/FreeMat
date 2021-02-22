% Test the vector subset with a cell array
function test_val = test_subset3
a = {1,2,3,4;5,6,7,8};
b = [3;4];
c = a(b);
test_val = test((c{1} == 2) & (c{2} == 6));
