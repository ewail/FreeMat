% Test the assignment of a value to a cell array as a vector-contents.
function test_val = test_assign2
a = {1,2,3,4,5};
test_val = 0;
try
a{5:6} = -1;
catch
test_val = 1;
end
