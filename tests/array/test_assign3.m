% Test the assignment of a value to a cell-array using content-based n-dim indexing
function test_val = test_assign3
a = {1,2;4,5};
test_val = 0;
try
a{2,1:2} = -1;
catch
test_val = 1;
end
