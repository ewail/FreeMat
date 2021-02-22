% Test the vector subset operator in an error condition
function test_val = test_subset2
a = [1,2,3,4];
b = [4,5,6];
test_val = 0;
try
c = a(b);
catch
test_val = 1;
end
