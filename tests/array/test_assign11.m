% Test for multiple assign with arrays
function test_val = test_assign11
a = zeros(3,4);
[b(1), b(2)] = size(a);
test_val = (b(1) == 3) & (b(2) == 4);
