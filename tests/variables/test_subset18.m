% Test the subset assign with an empty variable
function test_val = test_subset18
a = [];
b = 1:4;
a(:) = b;
test_val = (a(1) == 1) & (a(2) == 2) & (a(3) == 3) & (a(4) == 4);
