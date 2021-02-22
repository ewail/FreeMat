% Check the range function for double types with 3 args & vertical concatenation
function test_val = test_range4
range = 0.45:3.45;
range2 = [0.45,1.45,2.45,3.45];
test_val = test(range == range2);
