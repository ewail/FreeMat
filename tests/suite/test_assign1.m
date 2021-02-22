% Test the assignment of a value to a cell array as a vector-contents
function test_val = test_assign1
a = {1,2,3,4,5};
a{5} = -1;
test_val = test(a{5} == -1);
