% Check the range function for float types with 2 args & horizontal concatenation
function test_val = test_range3
range = 0.2f:2.2f;
range2 = [0.2f,1.2f,2.2f];
test_val = test(range == range2);
